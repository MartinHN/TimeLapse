//
//  ConfigGlove.h
//  Hey
//
//  Created by Atila on 29/10/15.
//
//

#ifndef Hey_ConfigGlove_h
#define Hey_ConfigGlove_h

#define LOCALPORT 2000
#define SERVERIP "200.200.1.1"
#define SERVERPORT 25000
#define APPNAME "HEYVIDEO"
#define LOCALIP "200.200.1.20"

#define NUMTOUCH 12
#define NUMFLEX 12


#define LOCK_AXE_TIME  0.7
#define MEDIAN_BUFFER_SIZE 0

typedef  enum TouchAction{
    GLOVE_ACTION_UP=0,
    GLOVE_ACTION_DOWN
};
typedef  int TouchButton;


#endif
