#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofHideCursor();
    ofSetVerticalSync(true);
    ofBackground(0);
    
    screenWidth = ofGetWidth();
    screenHeight = ofGetHeight();
    
    ofLog() << "loading HaikuSettings.xml" <<endl;
    
    //we load our settings file
    //if it doesn't exist we can still make one
    //by hitting the 's' key
    if( settings.loadFile("HaikuSettings.xml") ){
        ofLog() << "HaikuSettings.xml loaded!" << endl;
    }else{
        ofLog()<< "unable to load HaikuSettings.xml check data/ folder" << endl;
    }
    
    background.load(settings.getValue("BACKGROUND:IMAGE","paper.jpg"));
    
    string path = settings.getValue("VIDEO:PATH","movies/test");
    ofDirectory dir(path);
    dir.allowExt("mp4");
    dir.listDir();
    dir.sort();
    
    for (int i = 0; i<dir.size(); i++){
        string videoPath = ofToDataPath(dir.getPath(i),true);
        ofLog()<<videoPath<<endl<<endl;
        paths.push_back(videoPath);
    }
    ofLog()<<dir.size()<<endl;
    
    videoId = 0;
    string videoPath = ofToDataPath(dir.getPath(videoId),true);
    haiku.load(videoPath);
    haiku.setLoopState(OF_LOOP_NONE);
    haiku.play();
    
    //initialize OSC
    int oscInPort = settings.getValue("OSC:RECEIVER:PORT",6000);
    receiver.setup(oscInPort);
    string oscOutHost = settings.getValue("OSC:SENDER:HOST","192.168.1.255");
    int oscOutPort = settings.getValue("OSC:SENDER:PORT",5000);
    sender.setup(oscOutHost,oscOutPort);
    
    receiver.start();
    
    ofxOscMessage m;
    m.setAddress("/haiku_it/init");
    m.addStringArg("ready");
    sender.sendMessage(m);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    if (receiver.hasWaitingMessages()){
        ofxOscMessage receivedMessage;
        receiver.getNextMessage(receivedMessage);
        string addr = receivedMessage.getAddress();
        //ofLog()<< addr<<endl;
        
        if (addr.compare("/haiku/hexagram") == 0){ //if the OSC address corresponds to /haiku/hexagram
            int state = receivedMessage.getArgAsInt(0);
            videoId = receivedMessage.getArgAsInt(0);
        }
        
        else if (addr.compare("/hexagram/state") == 0){ //if the OSC address corresponds to /hexagram/state
            int state = receivedMessage.getArgAsInt(0);
            switch (state){
                case 0:
                    //haiku.setPaused(true);
                    break;
                case 1:
                    haiku.setPaused(false);
                    haiku.nextFrame();
                    haiku.play();
                    break;
                case 2:{
		    string videoPath = paths[videoId];
                    ofLog()<<videoPath<<endl;
                    haiku.loadAsync(videoPath);
                    haiku.setLoopState(OF_LOOP_NONE);
                    haiku.play();
                    break;
                }
            }
        }
    }
    
    if (haiku.isPlaying() && haiku.getCurrentFrame() == 140) haiku.setPaused(true);
    if (!haiku.getIsMovieDone()) haiku.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw(0,0,screenWidth,screenHeight);
    haiku.draw(0,0,screenWidth,screenHeight);

}

//--------------------------------------------------------------
