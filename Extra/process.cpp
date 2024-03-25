#include "process.h"

process::process(int PID, float arrival_time, float service_time){
	this->PID = PID;
	this->arrival_time = arrival_time;
	this->service_time = service_time;
};

