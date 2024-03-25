#ifndef EVENT_HEADER
#define EVENT_HEADER

#include "process.h"
#include "event.cpp"

//Event class
class event{
	public:
		float time;
		int type, PID;
		process* Process;
		//For type:
		//Arrival = 1, Departure = 2, Poll = 3

		//constructor for event object
		event(double time, int type, process* Process);
		
		//overloaded constructor for first event
		event();
};

#endif