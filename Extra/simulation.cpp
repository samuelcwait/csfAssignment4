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

		//Initialization events and processes
		process* INIT_process;
		event* INIT_event;
		process* pollProccess; //polling process, not actually used
		event* INIT_poll;


		//Generates and returns exponentially distributed interarrival times, service times based on given lambda 
		float exponentialRandom(float lambda){
			float random = rand() / (RAND_MAX + 1.0);
			return log(1 - random) / lambda;
		}

		//Constructor fo the simulation object
		Simulation(float lambda, float CPUServiceTime){

			clock = 0;
			processCount = 0;
			totalProcesses = 0;
			cpu_busy = false;
			numCPU_Busy = 0;
			numPolls = 0;
			numInRq = 0;
			numProcesses = 10000;
			turnarounds = 0;

			INIT_process = new process(0,0,0);
			INIT_event = new event(0,1,INIT_process);
			pollProccess =  new process(-1,0,0); //polling process, not actually used
			INIT_poll = new event(0.1,3,pollProccess);
			
			eventSchedule.push_back(INIT_event);
			eventSchedule.push_back(INIT_poll);
		
			float genTime = 0;

			for (int i = 0; i < numProcesses; i++){
				float interArrivalTime = exponentialRandom(lambda);
				genTime += interArrivalTime;
				process* newProcess = new process(i+1, genTime, exponentialRandom(1/CPUServiceTime));
				processList.push_back(newProcess);
			}
		}	

		//Creates and schedules event and adds it to the event queue
		void scheduleEvent(event* newEvent){
			//schedules new event at first if the new schedule is empty or the new events time is before the first events time
			if(eventSchedule.empty() || newEvent->time < eventSchedule.front()->time){ eventSchedule.push_front(newEvent); }				
			else{
				//standard c++ library list data structure implemented as a doubly linked list, so we need an event iterator 
				//to iterate through the linked list to find the index we need
				std::list<event*>::iterator i = eventSchedule.begin();
				//iterates through the schdule to find where the new event fits in the shedule timeline
				while (i != eventSchedule.end() && newEvent->time >= (*i)->time){ ++i; } 
				eventSchedule.insert(i, newEvent); //inserts the new event in the schdule based on the iterator
			}
		}

		//Handles Arrival Events (1)
		void handleArrival(event* currentEvent){
			if(cpu_busy == false){
				//if the cpu is not busy, then set it to busy
				cpu_busy = true;
				//create new departure event based on the service time + the current clock time for the current process that has arrived
				event* newDepart = new event(CPUServiceTime + clock, 2, currentEvent->Process);
				//schedule the departure event in the schedule
				scheduleEvent(newDepart);
			} else { readyQ.push(currentEvent->Process); } //if the cpu was busy, then we add the process to the ready queue 

			//create new process object for the next process in the ready queue
			process* nextProcess = readyQ.front();
			//checks if the ready queue is empty 
			if(nextProcess == nullptr){ processesEmpty = true; }
			else{
				//if ready queue is not empty, creates the arrival event for the next process
				event* newArrival = new event(nextProcess->arrival_time, 1, nextProcess);
				//schedules the arrival event for the next process
				scheduleEvent(newArrival);
			}
		}

		//Handles Deperture Events (2)
		void handleDeparture(event* currentEvent){
			if(readyQ.empty()){
				cpu_busy = false;
			} else {
				process* p = readyQ.front();
				readyQ.pop();
				event* newDepart = new event(CPUServiceTime + clock, 2, p);
				scheduleEvent(newDepart);
			}
			turnarounds += currentEvent->time-currentEvent->Process->arrival_time;
		}

		//Handles Polling Events (3)
		void handlePoll(event* currentEvent){
			if(!processesEmpty){
				numInRq += readyQ.size();
				numPolls++;
				if(cpu_busy == true){
					numCPU_Busy++;
				}
			}
			event* nextPoll = new event(clock + 0.1, 3, pollProccess);
			scheduleEvent(nextPoll);
		}

		event* getEvent(){
			event* event = nullptr;
			if(!eventSchedule.empty()){
				event = eventSchedule.front();
				eventSchedule.pop_front();
			}
			return event;
		}

		//Calculates the metrics needed for the simulation
		void calculateMetrics(){
			Tq = turnarounds / numProcesses;
			throughput = numProcesses / clock;
			rho = numCPU_Busy / numPolls;
			avgInRq = numInRq / numPolls;
		}

		//Outputs console readout for the metrics calculated
		void output(){

			std::cout << std::fixed << std::setprecision(5);
			std::cout << "-----------------------------------------\n";
			std::cout << "| Metric             | Value           |\n";
			std::cout << "-----------------------------------------\n";
			std::cout << "| Arrival Rate       | " << std::setw(15) << lambda << " |\n";
			std::cout << "| Service Time       | " << std::setw(15) << CPUServiceTime << " |\n";
			std::cout << "| Average Turnaround | " << std::setw(15) << Tq << " |\n";
			std::cout << "| Throughput         | " << std::setw(15) << throughput << " |\n";
			std::cout << "| CPU Utilization    | " << std::setw(15) << rho * 100 << "% |\n";
			std::cout << "| Average RQ Length  | " << std::setw(15) << avgInRq << " |\n";
			std::cout << "-----------------------------------------\n";

		}

		//logs the metrics in an output file based on the filepath entered by user
		void writeToLog(){
			std::cout << "Enter filepath for log: ";
			std::string filePath;
			std::cin >> filePath;
			std::cout << "\n";
			std::ofstream outFile(filePath, std::ios::app);
			if(!outFile){
				std::cerr << "Error: check filepath" << std::endl;
			} else {
				outFile << lambda << "," << CPUServiceTime << "," << Tq
				<< "," << throughput << "," << rho << "," << avgInRq << std::endl;
				outFile.close();
				std::cout << "written to log file" << std::endl; 
			}
		}

		//runs the simulation
		void run(){
			while(!eventQ.empty()){
				event* currentEvent =  getEvent();
				clock = currentEvent->time;
				switch(currentEvent->type){
					case 1:
						handleArrival(currentEvent);
						break;
					case 2:
						handleDeparture(currentEvent);
						break;
					case 3:
						handlePoll(currentEvent);
						break;
					default:
						std::cout << "\nError: invalid event type" << std::endl;
						break;
				}
			}
			//When done with simulation loop, calculate and output metrics to console and log file
			calculateMetrics();
			output();
			writeToLog();
		}

};