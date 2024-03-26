#ifndef PROCESS_H
#define PROCESS_H

class Process{
    public:
		int PID;
		float arrival_time;
		float service_time;		
		Process(int PID, float arrival_time, float service_time);
        Process();
};

#endif