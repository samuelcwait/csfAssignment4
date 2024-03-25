#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <queue>
#include<random>

#include "simulation.cpp"

void argChecktoConsole(int flag){
	switch(flag){
		case 1:
			std::cout << "Invalid argument number" << std::endl;
			break;
		case 2:
			std::cout << "Invalid argument type" << std::endl;
			break;
		case 3:
			std::cout << "Invalid argument range" << std::endl; 
	}
}

int checkNumArgs(int argc, char* argv[]){

	int flag = 0;
	if(argc > 4 || argc < 3){	flag = 1;	}
	else{ 
		try{ 
			float intaValue = std::stof(argv[1]);
			float floatValue = std::stof(argv[2]);
		}
		catch (const std::invalid_argument& ia){
			flag = 2;
		}
		catch ( const std::out_of_range& oor){
			flag = 3;
		}
	}
	argChecktoConsole(flag);
	return flag;
}


int main(int argc, char* argv[]){

	//checks argument number, range, and type 
	int flag = checkNumArgs(argc, argv);
	if(flag > 0){	return 1;	}

	float lambda = std::stod(argv[1]); //gets arrival rate from command line argument 
	float CPUServiceTime = std::stod(argv[2]);
	//double DiskServiceTime = std::stod(argv[3]); 	for OS project, not needed for CSF homework 4

	srand(time(0));
	
	Simulation sim(lambda, CPUServiceTime);
	sim.run();
}