/*
 *  ofxFFGLPlugin.h
 *
 *	Created by Daniel Berio 2010
 // Modified by Davide Maniˆ for OpenFrameworks 080
 // email: software@cogitamus.it
 */

#ifndef _OFXFFGLPLUGIN
#define _OFXFFGLPLUGIN

#include "ofMain.h"
#include "ofxFFGLWindow.h"
#include "FFGLPluginInfo.h"
#include "ofxFFGLParameter.h"

#include <FFGL.h>
#include <FFGLLib.h>
#include "FFGLPluginSDK.h"

const int MAX_INPUT_TEXTURES = 3;
const int NUM_PICTURES = 6;

class  ofFFGLPlugin: public CFreeFrameGLPlugin
{
public:
	ofFFGLPlugin(int minInputs = 1, int maxInputs = 1 );
	virtual ~ofFFGLPlugin();
	
	void initParameters();
	
  	DWORD InitGL(const FFGLViewportStruct *vp);
	DWORD DeInitGL();


	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
	void	setupInputTextures(ProcessOpenGLStruct* pGL);
	
	DWORD	SetTime(double time);
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	char*	GetParameterDisplay(DWORD dwIndex);
	
    /// override this to do something when a parameter has changed
    virtual void onParameterChanged(ofPtr<ofFFGLParameter>) {};

    vector < ofPtr<ofFFGLParameter> > parameters;
    int	getNumParameters() const { return parameters.size(); }
    ofPtr<ofFFGLParameter> getParameter(int index);
    
    //void initializeParameters();
    virtual void setup() {};
    virtual void update() {};
    virtual void draw() {};
    virtual void setupShaders() {};
    
protected:
	char _paramDisplay[128];
	
	bool isGLInitialized;
	
	ofPtr<ofFFGLWindow> _ofWin;
	
    // These functions add a parameter to the plugin, pass the address of the variable that will be automatically updated.
    // We could add normal FFGL parameter handling but this way even though it is more limited, it makes life much easier
    // Parameters MUST be added in the constructor or FFGL will ignore them.
    
    /// Add floating point parameter
    void addFloatParameter( const char * name, float * address, float min = 0.0f, float max = 1.0f );
    /// Add boolean parameter
    void addBoolParameter( const char * name, bool * address );
    /// Add event parameter ( is actually a boolean )
    void addEventParameter( const char * name, bool * address );
    /// Add string parameter
    void addStringParameter( const char * name, std::string * address );
    /// Add C String paramter ( a buffer of chars with termination character '\0' at the end )
    void addCStringParameter( const char * name, char * address );
    
    ofTexture * inputTextures[MAX_INPUT_TEXTURES];
    int numOfActiveTextures;
};

#endif
