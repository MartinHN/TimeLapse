//
//  GloveOSC.h
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//
// Class for handling glove connections with server and sending events in oF




#ifndef __ImMedia__GloveOSC__
#define __ImMedia__GloveOSC__

#include <stdio.h>

#include "ofxOsc.h"
#include "ofMain.h"
#include "GloveInstance.h"






class GloveOSC {
public:


    
    GloveOSC();
    ~GloveOSC();
    void init();
    
    
    void update(ofEventArgs & a);
    void draw(ofEventArgs & a);
    
    
    
    ofParameter<bool> isConnectedToServer;
    void registerOSC();
    void unregisterOSC();


    
    
private:
    static ofxOscReceiver reciever;
    void parseMessage();
    float lastPingTime;
    static ofxOscSender toServer;
    float lastACK;
    bool hasValidServer;
    void setConnected(bool & b);
    
};





#endif /* defined(__ImMedia__GloveOSC__) */
