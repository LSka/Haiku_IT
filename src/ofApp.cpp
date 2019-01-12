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
    
    // GL_REPEAT for texture wrap only works with NON-ARB textures //
    ofDisableArbTex();
    string bgTex = settings.getValue("BACKGROUND:IMAGE", "paper.jpg");
    texture1.load(bgTex); //Background texture ===To be subsituted by background videos!===
    texture1.getTexture().setTextureWrap( GL_REPEAT, GL_REPEAT );
    
    string path = settings.getValue("VIDEO:PATH","movies/test");
    ofDirectory dir(path);
    dir.allowExt("mp4");
    dir.listDir();
    dir.sort();
    
    
    for (int i = 0; i<dir.size(); i++){
        paths.push_back(dir.getPath(i));
    }
    ofLog()<<dir.size()<<endl;
    
    videoId = 0;
    string videoPath = ofToDataPath(dir.getPath(videoId),true);
    haiku.load(videoPath);
    haiku.setLoopState(OF_LOOP_NONE);
    haiku.play();
    
    //initialize the background plane
    bgPlane.set(screenWidth,screenHeight);
    bgPlane.setPosition(0,0,0);
    bgPlane.rotateDeg(90,0,0,1);
    
    
    
    //initialize lighting
    ofSetSmoothLighting(true);
    pointLight.setDiffuseColor( ofFloatColor(1, 1, 1) );
    pointLight.setSpecularColor( ofFloatColor(1.f, 1.f, 1.f));
    pointLight.setAttenuation(1.2,.0,0);
    pointLight.setPosition((ofGetWidth()*.5), ofGetHeight()/3, 500);
    
    pointLight2.setDiffuseColor( ofFloatColor(.95, .95, .65) );
    pointLight2.setSpecularColor( ofFloatColor(1.f, 1.f, 1.f));
    pointLight2.setPosition(-(ofGetWidth()*.5), ofGetHeight()/3, 500);
    
    pointLight3.setDiffuseColor( ofFloatColor(.95, .95, .65) );
    pointLight3.setSpecularColor( ofFloatColor(1.f, 1.f, 1.f));
    lightZ = 800; //pointLight3 Z position
    pointLight3.setPosition(0,0, lightZ);
    
    
    
    //set the materials
    // shininess is a value between 0 - 128, 128 being the most shiny //
    brickMaterial.setShininess( 80 );
    // the light highlight of the material //
    brickMaterial.setSpecularColor(ofFloatColor(.9,.9,.9));
    brickMaterial.setDiffuseColor(ofColor(1,1,1));
    brickMaterial.setAmbientColor(ofColor(0.8,0.8,0.8));
    
    bgMaterial.setShininess( 15 );
    // the light highlight of the material //
    bgMaterial.setSpecularColor(ofFloatColor(1,1,1));
    bgMaterial.setDiffuseColor(ofColor(80,80,80));
    bgMaterial.setAmbientColor(ofFloatColor(1,1,1));
    
    /* bgMovie.load("bgMovies/bgMovieTest_Center.mp4");
     bgMovie.setLoopState(OF_LOOP_NORMAL);
     bgMovie.play();
     */
    
    //set the camera position
    cam.setGlobalPosition({ 0,0,cam.getImagePlaneDistance(ofGetCurrentViewport()) });
    cam.rotateDeg(90,0,0,1);
    
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
        
        if (addr.compare("/hexagram/state") == 0){ //if the OSC address corresponds to /hexagram/state
            int state = receivedMessage.getArgAsInt(0);
            ofLog()<< addr<<": "<<state<<endl;
            switch (state){
                case 0:
                    break;
                case 1:
                    haiku.setPaused(false);
                    //haiku.setFrame(141);
                    haiku.play();
                    break;
                case 2:{
                    videoId = receivedMessage.getArgAsInt(1);
                    string videoPath = ofToDataPath(paths[videoId],true);
                    ofLog()<<"loading video #"<<videoId<<": "<<videoPath<<endl;
                    haiku.load(videoPath);
                    haiku.setLoopState(OF_LOOP_NONE);
                    if (haiku.isLoaded()) haiku.play();
                    break;
                }
            }
        }
    }
    int cF = haiku.getCurrentFrame();
    //ofLog()<<"current frame: "<<cF<<endl;
    if (haiku.isPlaying() && cF == 211 ) haiku.setPaused(true);
    if (!haiku.getIsMovieDone()) haiku.update();
    
    //move lights
    float time = ofGetElapsedTimef();
    float pulse1 = (sin(time*0.1)*1.5); //SIN(TIME*SPEED)*INTENSITY
    float pulse2 = (cos(time*0.15)*1.5);
    float pulse3 = (sin(time*0.1)*1.5);
    float noise = ofNoise(time*0.3);
    pointLight.setAttenuation(1.8+pulse1,0,0);
    pointLight2.setAttenuation(1.7+pulse2,0,0);
    pointLight3.setAttenuation(0.8+noise,0,0);
    pointLight3.setPosition(ofMap(noise,0.,1.,-140,140),pulse3*ofGetHeight(),lightZ);
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw(0,0,screenWidth,screenHeight);
    //haiku.draw(0,0,screenWidth,screenHeight);
    
    cam.begin();
    
    ofEnableDepthTest();
    
    ofEnableLighting();
    pointLight.enable();
    pointLight2.enable();
    pointLight3.enable();
    
    bgMaterial.begin();
    //bgMovie.getTexture().bind();
    ofSetColor(255,255,255);
    if(haiku.isLoaded()) { //Don't draw if the video isn't ready
        haiku.bind();
        bgPlane.draw();
        haiku.unbind();
    }
    //bgMovie.getTexture().unbind();
    bgMaterial.end();
    
    ofDisableLighting();
    ofDisableDepthTest();
    
    cam.end();

}

//--------------------------------------------------------------
