#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <list>
#include "Process.h"

class Event{
	public:
		float time;
		int type;
		Process* process;

		//constructor for event object
		Event(float time, int type, Process* Process);
		
		//overloaded constructor for first event
		Event();
};

class Simulation{
	public:
		
		//event queue implemented as a doubly linked list using standard c++ list library
		std::list<Event*> eventQ;
		//ready quque implemented as a doubly linked list using standard c++ list library
		std::list<Process*> readyQ;

		double lambda, CPUServiceTime; //lambda = arrival rate

		float clock;	//global simulator clock
		int processCount;
		int totalProcesses;
		bool cpu_busy; //boolean for whether cpu is busy
		float numCPU_Busy; //running total of times the cpu was busy during a poll

		float numPolls; //number of poll events taken for metrics
		float numInRq; //running total of processes in ready queue when polls are taken for metrics
		
		float numProcesses; //number of processes for simluation is 10,000
		float turnarounds; //running total of individual processes turnaround time 
		bool processesEmpty;
		bool eventsEmpty;
		float Tq, throughput, rho, avgInRq; //for metrics later

		//Initialization events and processes
		Process* INIT_process;
		Event* INIT_event;
		Process* pollProccess; //polling process, not actually used
		Event* INIT_poll;

		//Generates and returns exponentially distributed interarrival times, service times based on given lambda 
		float exponentialRandom(float lambda);

		//Constructor fo the simulation object
		Simulation(float lambda, float CPUServiceTime);

		//Creates and schedules event and adds it to the event queue
		void scheduleEvent(Event* newEvent);

		//Handles Arrival Events (1)
		void handleArrival(Event* currentEvent);

		//Handles Deperture Events (2)
		void handleDeparture(Event* currentEvent);

		//Handles Polling Events (3)
		void handlePoll(Event* currentEvent);

		Event* getEvent();

		//Calculates the metrics needed for the simulation
		void calculateMetrics();

		//Outputs console readout for the metrics calculated
		void output();

		//logs the metrics in an output file based on the filepath entered by user
		void writeToLog();

		//runs the simulation
		void run();

};