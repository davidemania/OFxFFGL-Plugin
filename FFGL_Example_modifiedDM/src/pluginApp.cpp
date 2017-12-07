// **************************************************************************
//
// ofxFFGLPlugin for OpenFrameworks 080 - experimental version, jan 15 2014
// Modified by Davide Maniˆ for OpenFrameworks 080
// email: software@cogitamus.it
// based on ofxFFGLPlugin by Daniel Berio
//
// Tested on OSX (XC 5.02) only!
//
// Write user code here
//
// setup() update() and draw() work just like the old ones in testApp()
//
// event handlers have been removed, they were not being used anyway
// Handle parameter changes in onParameterChanged()
//
// **************************************************************************

#include "pluginApp.h"
#include "shaders.h"

#include <dlfcn.h> // for getExecutableFile() and related

extern CFFGLPluginInfo* g_CurrPluginInfo;

pluginApp::pluginApp(int minInputs, int maxInputs)
{
	// add needed FFGL parameters here
	
	numRepeats = 2; // default value for parameter
	useShader = false;
    
	addFloatParameter("Repetitions",	// name of parameter ( as it will appear in host )
                      &numRepeats,      // address of the float this parameter will point to
                      1,				// minimum value
                      20				// maximum value
					  );
    
    addBoolParameter("Enable Shader", &useShader);
    
    initParameters(); // Setup parameters with host
}

// **************************************************************************

pluginApp::~pluginApp()
{
    
}

const char* getExecutableFile()
{
    Dl_info exeInfo;
    dladdr((void*) getExecutableFile, &exeInfo); // quite a weird construct but totally legit :)
    return exeInfo.dli_fname;
}

// **************************************************************************
// Called by instantiateGL
// **************************************************************************

void pluginApp::setup()
{
    // Get the absolute location of the executable file in the bundle.
    string strExecFile(getExecutableFile());
    
    // Strip out the filename to just get the path
    int found = strExecFile.find_last_of("/");
    string strPath = strExecFile.substr(0, found) + "/data/";
		
    // Change the working directory to that of the data folder
	//chdir(strPath.c_str());
    ofSetDataPathRoot(strPath.c_str());
    
	myFont.loadFont("font/Ubuntu-R.ttf", 30);

    // Copy plugin Major and minor version from data set in main.cpp
    // To be displayed onscreen as test and debug info
    
    char buffer[10];
    sprintf(buffer, "%d.%03d", g_CurrPluginInfo->GetPluginExtendedInfo()->PluginMajorVersion,
            g_CurrPluginInfo->GetPluginExtendedInfo()->PluginMinorVersion);
    debugText = string("FFGL Test 2014\n plugin V") + string(buffer);
    
    //ofBackground(255,255,255);
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
}

// **************************************************************************
// Gets called immediately after setup()
// **************************************************************************

void pluginApp::setupShaders()
{
    // use this to load shaders from strings hardcoded in shaders.h
    
    prepareShaderCode(defaultVertexShader);
    prepareShaderCode(defaultFragmentShader);
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, defaultVertexShader);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, defaultFragmentShader);
    
    shader.linkProgram();
    
    // Use this to load shaders from files in /data
    
    //shader.load("shader.vert.glsl", "shader.frag.glsl");
    
    //bool ok = shader.isLoaded();
}

void pluginApp::update()
{

}

// **************************************************************************

void pluginApp::draw()
{
	// input textures from host are stored here
	ofTexture * tex = inputTextures[0];
	
    
	if( !tex || !myFont.isLoaded())
		return;
    
	int nrep = numRepeats;
    
    // Draw received texture multiple times (tiled), according to parameter
    
	float w = (float)ofGetWidth() / nrep;
	float h = (float)ofGetHeight() / nrep;
	
    fbo.begin();
	for( int y = 0; y < nrep; y++ )
    {
		for( int x = 0; x < nrep; x++ )
		{
			tex->draw(w*x,h*y,w,h);
		}
	}

    // Draw a simple geometric shape
    
	ofSetColor(255,0,255);
	ofNoFill();
    myFont.drawString(debugText, 100, 200);
	ofRect(20, 20, ofGetWidth()-40, ofGetHeight()-40);
    fbo.end();
    
    if(useShader)
    {
        shader.begin();
        shader.setUniformTexture("texture0", *tex, 0);
    }
    ofSetColor( 255, 255, 255 );
    fbo.draw(0, 0);
    
    if(useShader)
    {
        shader.end();
    }
}

// **************************************************************************

void pluginApp::onParameterChanged(ofPtr<ofFFGLParameter>)
{
    // Do something if you wish
}

// **************************************************************************
// **************************************************************************
