#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
//#include "ofRPIVideoPlayer.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    int screenWidth;
    int screenHeight;
    int videoId;

    float lightZ;
    
    ofxXmlSettings settings;
		
    ofxOscReceiver receiver;
    ofxOscSender sender;
    
    ofImage background;
    //ofRPIVideoPlayer haiku;
    
    vector<string> paths;
    ofVideoPlayer haiku;
    
    ofLight pointLight;
    ofLight pointLight2;
    ofLight pointLight3;
    
    ofMaterial brickMaterial;
    ofMaterial bgMaterial;
    
    ofImage texture1;
    ofPlanePrimitive bgPlane;

    
    ofCamera cam;
    
};
