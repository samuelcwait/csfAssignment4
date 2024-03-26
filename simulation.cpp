#include "simulation.h"

Event::Event(float time, int type, Process* process)
{ 
	this->time = time;
	this->type = type;
	this->process = process; 
}

Event::Event()
{
	time = 0;
	type = 0;
	process = nullptr;
}


Simulation::Simulation(float lambda, float CPUServiceTime)
{
	clock = 0;
	processCount = 0;
	totalProcesses = 0;
	cpu_busy = false;
	eventsEmpty = false;
	numCPU_Busy = 0;
	numPolls = 0;
	numInRq = 0;
	numProcesses = 10000;
	turnarounds = 0;
	INIT_process = new Process(0,0,0);
	INIT_event = new Event(0,1,INIT_process);
	pollProccess =  new Process(-1,0,0); //polling process, not actually used
	INIT_poll = new Event(0.1,3,pollProccess);
	
	eventQ.push_back(INIT_event);
	eventQ.push_back(INIT_poll);

	float genTime = 0;

	for (int i = 0; i < numProcesses; i++){
		float interArrivalTime = exponentialRandom(lambda);
		genTime += interArrivalTime;
		Process* newProcess = new Process(i+1, genTime, exponentialRandom(1/CPUServiceTime));
		processList.push_back(newProcess);
	}
}

float Simulation::exponentialRandom(float lambda){
			float random = rand() / (RAND_MAX + 1.0);
			return -log(1 - random) / lambda;
		}

void Simulation::scheduleEvent(Event* newEvent){
			//schedules new event at first if the new schedule is empty or the new events time is before the first events time
			if(eventQ.empty() || newEvent->time < eventQ.front()->time)
				{ eventQ.push_front(newEvent); }				
			else{
				
				//standard c++ library list data structure implemented as a doubly linked list, so we need an event iterator 
				//to iterate through the linked list to find the index we need
				std::list<Event*>::iterator i = eventQ.begin();

				//iterates through the event queue to find where the new event fits in the shedule timeline
				while (i != eventQ.end() && newEvent->time >= (*i)->time){ ++i; } 
				eventQ.insert(i, newEvent); //inserts the new event in the event queue based on the iterator
			}
		}

void Simulation::handleArrival(Event* currentEvent){

			Event* newArr = new Event();
			Event* newDepart = new Event();

			if(cpu_busy == false){
				//if the cpu is not busy, then set it to busy
				cpu_busy = true;
				//create new departure event based on the service time + the current clock time for the current process that has arrived
				newDepart = new Event(CPUServiceTime + clock, 2, currentEvent->process);
				//schedule the departure event in the schedule
				scheduleEvent(newDepart);
			} 
			else { readyQ.push_back(currentEvent->process); } //if the cpu was busy, then we add the process to the ready queue 

			//create new process object for the next process in the ready queue
			Process* nextProcess = nullptr;
			if(!readyQ.empty()){
				nextProcess = readyQ.front();
				readyQ.pop_front();
			}

			//checks if the ready queue is empty 
			if(nextProcess == nullptr){ processesEmpty = true; }
			else{
				//if ready queue is not empty, creates the arrival event for the next process
				newArr = new Event(nextProcess->arrival_time, 1, nextProcess);
				//schedules the arrival event for the next process
				scheduleEvent(newArr);
			}
		}

//Handles Deperture Events (2)
void Simulation::handleDeparture(Event* currentEvent){
	if(readyQ.empty()){
		cpu_busy = false;
	} else {
		Process* p = readyQ.front();
		readyQ.pop_front();
		Event* newDepart = new Event(CPUServiceTime + clock, 2, p);
		scheduleEvent(newDepart);
	}
	turnarounds += currentEvent->time-currentEvent->process->arrival_time;
}

//Handles Polling Events (3)
void Simulation::handlePoll(Event* currentEvent){
	if(!processesEmpty){
		numInRq += readyQ.size();
		numPolls++;
		if(cpu_busy == true){
			numCPU_Busy++;
		}
		Event* nextPoll = new Event(clock + 0.1, 3, pollProccess);
		scheduleEvent(nextPoll);
	}
}

Event* Simulation::getEvent(){
	Event* event = nullptr;
	if(!eventQ.empty()){
		event = eventQ.front();
		eventQ.pop_front();
	}
	return event;
}

//Calculates the metrics needed for the simulation
void Simulation::calculateMetrics(){
	Tq = turnarounds / numProcesses;
	throughput = numProcesses / clock;
	rho = numCPU_Busy / numPolls;
	avgInRq = numInRq / numPolls;
}

//Outputs console readout for the metrics calculated
void Simulation::output(){

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
void Simulation::writeToLog(){
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
void Simulation::run(){
	std::cout << "Simulation Start" << std::endl;
	int i = 0; 
	while(i < numProcesses){
		//std::cout << "event #" << i << std::endl; 
		if(eventQ.empty()){
			eventsEmpty = true;
		}
		Event* currentEvent = getEvent();
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
		i++;
	}
	std::cout << "Simulation End" << std::endl;
	//When done with simulation loop, calculate and output metrics to console and log file
	calculateMetrics();
	output();
	writeToLog();
}


