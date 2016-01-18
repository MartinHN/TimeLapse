//
//  Draggable.cpp
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//

#include "GloveInteract.h"

GloveInstance * GloveInteract::curGlove = NULL;



GloveInteract::GloveInteract(){
    gloveEventsPriority = 0;
//    gloveEventsPriority = 0;
    GloveInteract::addListeners(gloveEventsPriority);
//    gloveEventsPriority = 0;

    
}

void GloveInteract::addListeners(int & priority){
    removeGloveListeners(lastGloveEventsPrio);
    
    ofAddListener( GloveInstance::cursor2DEvent,this,&GloveInteract::cursor2DMoved,OF_EVENT_ORDER_APP + gloveEventsPriority);
    ofAddListener(GloveInstance::touchEvent,this,&GloveInteract::touch,OF_EVENT_ORDER_APP + gloveEventsPriority);
    ofAddListener(GloveInstance::relativeOrientationEvent,this,&GloveInteract::relativeMoved,OF_EVENT_ORDER_APP + gloveEventsPriority);
    ofAddListener(GloveInstance::swipeEvent,this,&GloveInteract::swiped,OF_EVENT_ORDER_APP + gloveEventsPriority);
    
    ofAddListener(ofEvents().update, this, &GloveInteract::update);
    lastGloveEventsPrio = priority;
}



GloveInteract::~GloveInteract(){
    removeGloveListeners(gloveEventsPriority);

    
}


void GloveInteract::removeGloveListeners(int priority){
    ofRemoveListener( GloveInstance::cursor2DEvent,this,&GloveInteract::cursor2DMoved,OF_EVENT_ORDER_APP + priority);
    ofRemoveListener(GloveInstance::touchEvent,this,&GloveInteract::touch,OF_EVENT_ORDER_APP + priority);
    ofRemoveListener(GloveInstance::relativeOrientationEvent,this,&GloveInteract::relativeMoved,OF_EVENT_ORDER_APP + priority);
    ofRemoveListener(GloveInstance::swipeEvent,this,&GloveInteract::swiped,OF_EVENT_ORDER_APP + priority);
    
    
    ofRemoveListener(ofEvents().update, this, &GloveInteract::update);
}





