/*
 *  ofxFFGLPlugin.cpp
 *  FFGLExample
 *
 *  Created by ensta on 2/4/10.
 *  Copyright 2010 Daniel Berio. All rights reserved.
 *
 *  http://www.enist.org
 // Modified by Davide Maniˆ for OpenFrameworks 080
 // email: software@cogitamus.it
 */

#include "ofxFFGLPlugin.h"
#include "ofAppRunner.h"

ofFFGLPlugin::ofFFGLPlugin(int minInputs, int maxInputs)
{
	// Input properties
	SetMinInputs(minInputs);
	SetMaxInputs(maxInputs);
	
    numOfActiveTextures = 0;
    
    //initializeParameters();
	//initParameters();
    
	for( int i = 0; i < MAX_INPUT_TEXTURES; i++ )
		inputTextures[i] = NULL;
    
	isGLInitialized = false;
}

ofFFGLPlugin::~ofFFGLPlugin()
{
    for( int i = 0; i < MAX_INPUT_TEXTURES; i++ )
	{
		if( inputTextures[i] )
		{
			inputTextures[i]->texData.textureID = 0; // prevent OF from deleting the actual GL texture since it is allocated by host
			delete inputTextures[i];
		}
	}
    
	isGLInitialized = false;
}
	

void ofFFGLPlugin::initParameters()
{
	for(int i = 0; i < parameters.size(); i++ )
	{
		ofPtr<ofFFGLParameter> param = parameters[i];
		
		switch( param->getType() )
		{
			case PARAM_BOOL:
			{
				SetParamInfo(i, param->getName(), FF_TYPE_BOOLEAN, param->getBool());
				break;
			}
			
			case PARAM_FLOAT:
			{
				SetParamInfo(i, param->getName(), FF_TYPE_STANDARD, param->getFloat01());
				break;
			}
			
			case PARAM_CSTRING:
			case PARAM_STRING:
			{
				SetParamInfo(i, param->getName(),FF_TYPE_TEXT, param->getString());
				break;
			}

			case PARAM_EVENT:
			{
				SetParamInfo(i, param->getName(), FF_TYPE_EVENT, param->getBool() );
				break;
			}
			default: ;
		}
	}
}

DWORD ofFFGLPlugin::InitGL(const FFGLViewportStruct *vp)
{
    _ofWin = ofPtr<ofFFGLWindow>(new ofFFGLWindow());
    ofSetupOpenGL(_ofWin, vp->width, vp->height, OF_WINDOW);
    
    _ofWin->initializeWindow(); // does nothing at the moment
    
    ofSeedRandom();
    ofResetElapsedTimeCounter();
    
    glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		//ofLogError("ofAppRunner") << "couldn't init GLEW: " << glewGetErrorString(err);
	}

	//Default colors etc are now in ofGraphics - ofSetupGraphicDefaults
    ofSetupGraphicDefaults();
	isGLInitialized = true;
    
    setup();
    setupShaders();
    
	return FF_SUCCESS;
}

DWORD ofFFGLPlugin::DeInitGL()
{
	isGLInitialized = false;
	return FF_SUCCESS;
}
	
	
void ofFFGLPlugin::setupInputTextures(ProcessOpenGLStruct* pGL)
{
    numOfActiveTextures = pGL->numInputTextures;
    
	for(int i = 0; i < numOfActiveTextures; i++ )
	{
		// create texture here if it isnt there...
		if( !inputTextures[i] )
			inputTextures[i] = new ofTexture();
			
		FFGLTextureStruct &tex = *(pGL->inputTextures[i]);
		ofTexture * ofTex = inputTextures[i];
		
		// adapted from ofQC by vade.
		ofTex->texData.textureID = tex.Handle;
		ofTex->texData.textureTarget = GL_TEXTURE_2D; // GL_TEXTURE_RECTANGLE_ARB;
		ofTex->texData.width = tex.Width;
		ofTex->texData.height = tex.Height;
		ofTex->texData.bFlipTexture = true;
		ofTex->texData.tex_w = tex.HardwareWidth;
		ofTex->texData.tex_h = tex.HardwareHeight;
		ofTex->texData.tex_t = ((float)tex.Width)/ tex.HardwareWidth;
		ofTex->texData.tex_u = ((float)tex.Height)/ tex.HardwareHeight;
		ofTex->texData.glTypeInternal = GL_RGBA;  // this is just a guess...
		//ofTex->texData.glType = GL_RGBA;
		ofTex->texData.bAllocated = true;
	}
}

DWORD ofFFGLPlugin::ProcessOpenGL(ProcessOpenGLStruct* pGL)
{
	if(!isGLInitialized)
		return FF_SUCCESS;
		
	update();
    
	setupInputTextures(pGL);
	
//	GLint vp[4];
//	glGetIntegerv(GL_VIEWPORT,vp);
	
	GLint mmode;
	glGetIntegerv(GL_MATRIX_MODE,&mmode);
	
	//push all matrices
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// set up coordinate system based on our proxy window.
	glOrtho(0.0, _ofWin->windowSize.x, _ofWin->windowSize.y, 0.0, -1.0, 1.0);

    //glOrtho(0.0, 640, 480, 0.0, -1.0, 1.0);
    
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// this could be optimized...alot
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// draw
	draw();
	
	glPopAttrib();
	
	//reset all matrices
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glMatrixMode(mmode);
	
	// we reset the host fbo id here
	// in case we have been rendering offscreen in the plugin.
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,pGL->HostFBO);
	
	// TODO it may be necessary to even do this:
	// but it's a bit of a hack....
	
	/*if( pGL->HostFBO )
	{
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	else 
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	}
	*/
	
	// ... and it would be better if host handles these things 

	return FF_SUCCESS;
}


DWORD	ofFFGLPlugin::SetTime(double time)
{
	// todo.....
	 return FF_SUCCESS; 
}

DWORD ofFFGLPlugin::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;
  
    ofPtr<ofFFGLParameter> v;
    if(dwIndex < parameters.size())
    {
        v = parameters[dwIndex];
    }
    else
	{
		return FF_FAIL;
	}
	
	switch(v->getType())
	{
		
		case PARAM_FLOAT:
		{
			float val = (v->getFloat() - v->getMin()) / (v->getMax()-v->getMin());
			 *((float *)(unsigned)&dwRet) = val;
			return dwRet;
		}	
		
		case PARAM_CSTRING:
		case PARAM_STRING:
		{
			const char * str = v->getString();
			dwRet = (DWORD)str;
			return dwRet;
		}
		
		case PARAM_BOOL:
		case PARAM_EVENT:
		{
			*((float *)(unsigned)&dwRet) = v->getBool();
			return dwRet;
		}
		
		
		default:
		{
			return FF_FAIL;
		}
	}
	
	return FF_FAIL;
}

DWORD ofFFGLPlugin::SetParameter(const SetParameterStruct* pParam)
{
    ofPtr<ofFFGLParameter> v;
    if(pParam->ParameterNumber < parameters.size())
    {
        v = parameters[pParam->ParameterNumber];
    }
    else
	{
		return FF_FAIL;
	}
	
	switch(v->getType())
	{
		case PARAM_FLOAT:
		{
			float val =  *((float *)(unsigned)&(pParam->NewParameterValue));
			
			v->setFloat( v->getMin() + val*(v->getMax()-v->getMin()) );
			onParameterChanged(v);
			return FF_SUCCESS;
		}	
		
		case PARAM_CSTRING:
		case PARAM_STRING:
		{
			char * str = (char*)(pParam->NewParameterValue);
			v->setString(str);
			onParameterChanged(v);

			return FF_SUCCESS;
		}
		
		case PARAM_BOOL:
		case PARAM_EVENT:
		{
			float val =  *((float *)(unsigned)&(pParam->NewParameterValue));
			v->setBool((bool)val);
			onParameterChanged(v);
			return FF_SUCCESS;
		}
		
		default:
		{
			//debugPrint("ofFFGLPlugin::SetParameter  unknown parameter type for %s\n",v->getName().str);
			return FF_FAIL;
		}
	}
	
	return FF_FAIL;
}

char* ofFFGLPlugin::GetParameterDisplay(DWORD dwIndex)
{
    ofPtr<ofFFGLParameter> v;
    if(dwIndex < parameters.size())
    {
        v = parameters[dwIndex];
    }
    else
	{
		return NULL; // FF_FAIL;
	}
	
	switch(v->getType())
	{
		case PARAM_FLOAT:
			sprintf(_paramDisplay,"%0.2f",v->getFloat());
			return _paramDisplay;
			
		case PARAM_BOOL:
		case PARAM_EVENT:
			sprintf(_paramDisplay,"%d",v->getBool());
			return _paramDisplay;
			
		case PARAM_STRING:
		case PARAM_CSTRING:
			return (char*)v->getString();
        default: ;
	}
	
	return NULL;
}

void ofFFGLPlugin::addFloatParameter( const char * name, float * address, float min , float max  )
{
	ofPtr<ofFFGLParameter> p = ofPtr<ofFFGLParameter>(new ofFFGLParameter());
	p->initFloat(name,address,min,max);
	parameters.push_back(p);
}

void ofFFGLPlugin::addBoolParameter( const char * name, bool * address )
{
	ofPtr<ofFFGLParameter> p = ofPtr<ofFFGLParameter>(new ofFFGLParameter());
	p->initBool(name,address);
	parameters.push_back(p);
}


void ofFFGLPlugin::addEventParameter( const char * name, bool * address )
{
	ofPtr<ofFFGLParameter> p = ofPtr<ofFFGLParameter>(new ofFFGLParameter());
	p->initEvent(name,address);
	parameters.push_back(p);
}

void ofFFGLPlugin::addCStringParameter( const char * name, char * address )
{
	ofPtr<ofFFGLParameter> p = ofPtr<ofFFGLParameter>(new ofFFGLParameter());
	p->initCString(name,address);
	parameters.push_back(p);
}

void ofFFGLPlugin::addStringParameter( const char * name, std::string * address )
{
	ofPtr<ofFFGLParameter> p = ofPtr<ofFFGLParameter>(new ofFFGLParameter());
	p->initString(name,address);
	parameters.push_back(p);
}

