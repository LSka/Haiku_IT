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

    
    ofxXmlSettings settings;
		
    ofxOscReceiver receiver;
    ofxOscSender sender;
    
    ofImage background;
    //ofRPIVideoPlayer haiku;
    
    vector<string> paths;
    ofVideoPlayer haiku;
    
};
