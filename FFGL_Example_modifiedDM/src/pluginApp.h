#ifndef FFGL_PLUGIN_APP
#define FFGL_PLUGIN_APP

#include "ofxFFGLPlugin.h"

class pluginApp : public ofFFGLPlugin
{
public:
    pluginApp(int minInputs = 1, int maxInputs = 1);
	virtual ~pluginApp();
    
    // do something when a parameter has changed
    virtual void onParameterChanged(ofPtr<ofFFGLParameter>);
    
    void setup();
    void update();
    void draw();
    void setupShaders();
    
    ofShader shader;
    ofFbo fbo;
    ofTrueTypeFont myFont;
    
private: // Insert here declarations needed in your implementation

    string debugText;
    
    float numRepeats;
    bool useShader;
};

#endif // FFGL_PLUGIN_APP

	
