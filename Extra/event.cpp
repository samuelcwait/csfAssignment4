#include "event.h"

event::event(float time, int type, process* Process){ 
	this->time = time;
	this->type = type;
	this->Process = Process; 
};

//overloaded constructor for first event
event::event(){
	this->time = 0;
	this->type = 1;
	this->PID = 1;
	this->Process = nullptr; 
}
