#ifndef PROCESS_H
#define PROCESS_H

//Process class
class process{
	public:
		process(int PID, float arrival_time, float service_time);
		int PID;
		float arrival_time;
		float service_time;		
};

#endif