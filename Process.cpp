#include "Process.h"

Process::Process(int PID, float arrival_time, float service_time)
{
	this->PID = PID;
	this->arrival_time = arrival_time;
	this->service_time = service_time;
}

Process::Process()
{
	PID = 0;
	arrival_time = 0.0;
	service_time = 0.0;		
}

