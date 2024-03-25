#ifndef SIM_H
#define SIM_H

#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <math.h>
#include <random>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <list>

class process{
	public:
		process(int PID, float arrival_time, float service_time){
			this->PID = PID;
			this->arrival_time = arrival_time;
			this->service_time = service_time;
		}

		int PID;
		float arrival_time;
		float service_time;		
};

class event{
	public:
		float time;
		int type, PID;
		process* Process;
		//For type:
		//Arrival = 1, Departure = 2, Poll = 3

		//constructor for event object
		event(float time, int type, process* Process){ 
			this->time = time;
			this->type = type;
			this->Process = Process; 
		}
		
		//overloaded constructor for first event
		event(){
			this->time = 0;
			this->type = 1;
			this->PID = 1;
			this->Process = nullptr; 
		}
};

class Simulation{
	public:
	
		std::queue<event*> eventQ; //event queue
		std::queue<process*> readyQ; //ready queue
		
		//schdule for the events as a list, different from the event queue so we can schedule events where needed
		std::list<event*> eventSchedule;

		//list of processess as a list
		std::list<process*> processList;


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
		float Tq, throughput, rho, avgInRq; //for metrics later

		process* pollProccess; //polling process, not actually used

		//Generates and returns exponentially distributed interarrival times, service times based on given lambda 
		double exponentialRandom(float lambda);

		//Constructor fo the simulation object
		Simulation(float lambda, float CPUServiceTime);

		//Creates and schedules event and adds it to the event queue
		void scheduleEvent(event* newEvent);

		//Handles Arrival Events (1)
		void handleArrival(event* currentEvent);

		//Handles Deperture Events (2)
		void handleDeparture(event* currentEvent);

		//Handles Polling Events (3)
		void handlePoll(event* currentEvent);

		//Calculates the metrics needed for the simulation
		void calculateMetrics();

		//Outputs console readout for the metrics calculated
		void output();

		//logs the metrics in an output file based on the filepath entered by user
		void writeToLog(std::string filePath);

		//runs the simulation
		void run();
};

#endif