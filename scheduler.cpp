// CMSC 312 Project - Part 1
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>

#include "scheduler.h"
using namespace std;


Scheduler::Scheduler(){
    this->scheduling_type = 0;
    this->quantum = 5;
    this->rr_count = 0;
}



void Scheduler::print(){
    cout << endl << "--------------------- Main Memory ---------------------" << endl;
	cout << "Frame Number -- Page Number 	" << endl;
	for(int i = 0; i < NUM_OF_FRAMES;i++){
		cout << " " << i << ":	     	 " << Main_Memory[i] << endl;
	}
    return;
}


void Scheduler::ShortestFirst(vector<Process>& PCB){
    vector<Process>::iterator it = PCB.begin();
	while(PCB.size() > 0){	// While the PCB isnt empty, run.
	if(it == PCB.end()){it = PCB.begin();} // If it reaches end of PCB, Loop back to start.


    switch(it->GetState()){


	case Process::New:	// Default state, Gets set to ready when running for the first time
		for(int i = 0; i < it->GetNumOfInstr(); i++){	// checks based on the number of instructions by
			for(int j = 0; j < NUM_OF_FRAMES; j++){		// looping through Memory to see if an open spot.
				if(Virtual_Memory[j] == -1){			// If yes, place in process number and set process to ready.
					Virtual_Memory[j] = it->GetNameNumber();
					it->SetPageTable(i,0,j);
					it->SetPageTable(i,1,1);
					enough_memory = true;
					break;
				}
				else{
					enough_memory = false;
				}
			}	
		}
		if(enough_memory){
			it->SetState(Process::Ready);
			it->printState();
			cout << endl << "--------------------- Virtual Memory ---------------------" << endl;
			cout << "Frame Number -- Process Number 	" << endl;
			for(int i = 0; i < NUM_OF_FRAMES;i++){
				cout << " " << i << ":	     	 " << Virtual_Memory[i] << endl;
			}
			break;
		}
		it->SetState(Process::New);
		it->printState();
		break;
	case Process::Ready:	// Process is on CPU ready to move to running when scheduled.
		for(int i = 0;i < NUM_OF_FRAMES;i++){ // Loop through Main Memory to find page of current process.
			if(it->GetPageTable(it->GetPageNum(),0) == Main_Memory[i]){
				it->SetInMem(true);
				break;
			}
		}
		if(it->GetInMem() == false){ 
			for(int i = 0;i < NUM_OF_FRAMES;i++){ // If its not in memory, loop to find empty page.
				if(Main_Memory[i] == -1){
					Main_Memory[i] = it->GetPageTable(it->GetPageNum(),0);
					MM_full = false;
					break;
				}
				if(i == (NUM_OF_FRAMES - 1)){
					MM_full = true;
				}
			}
			if(MM_full == true){
				Main_Memory[full_mem_count] = it->GetPageTable(it->GetPageNum(),0);
				full_mem_count += 1;
			}
		}
		it->SetState(Process::Running);
		it->printState();
		if(MM_print == 1){
			cout << endl << "--------------------- Main Memory ---------------------" << endl;
			cout << "Frame Number -- Page Number 	" << endl;
			for(int i = 0; i < NUM_OF_FRAMES;i++){
				cout << " " << i << ":	     	 " << Main_Memory[i] << endl;
			}
		}
		break;
	case Process::Running:	// Ready -> Running. Stays if Calculate, moves to waiting if I/O or fork.
		if((it->GetInstructionType()) == 1 && it->GetListSize() != 0)	// Checks if current instruction is I/O
		{
			it->SetState(Process::Waiting);
			it->printState();
			break;
		}	
		if(it->GetInstructionType() == 3)	// Checks if Critical Section
		{
			if(critical == false){critical = true;}
			else{critical = false;}
			it->decrement_cycle();
			break;
		}
		if(it->decrement_cycle())
		{
			it->print();
		}
		else if(it->GetListSize() != 0)
		{
			it->SetState(Process::Ready);
			it->printState();
		}
		else
		{
			it->SetState(Process::Terminated);
			it->printState();
		}
		break;
	case Process::Waiting:	// State for I/O is called or if waiting on child process from fork.
		if((it->GetInstructionType()) == 0){it->SetState(Process::Ready);it->printState();break;}
		if(it->decrement_cycle())
		{
			it->print();
		}
		else
		{
			it->SetState(Process::Ready);
			it->printState();
		}
		break;
	case Process::Terminated:	// State called right before deleted from PCB.
		for(int i = 0; i < NUM_OF_FRAMES; i++){ // Goes through Main Memory and clears
			for(int j = 0; j < NUM_OF_FRAMES; j++){ // Runs through page table
				if(Main_Memory[i] == it->GetPageTable(j,0) && it->GetPageTable(j,1) == 1){
					Main_Memory[i] = -1;
				}
			}
		}
		for(int i = 0; i < NUM_OF_FRAMES; i++){	// Goes through page table, clears memory, and then clears page table.
			if(it->GetPageTable(i,1) == 1){
				Virtual_Memory[it->GetPageTable(i,0)] = -1;
				it->SetPageTable(i,1,0);
			}
		}
		cout << endl << "--------------------- Virtual Memory ---------------------" << endl;
		cout << "Frame Number -- Process Number 	" << endl;
		for(int i = 0; i < NUM_OF_FRAMES;i++){
			cout << " " << i << ":	     	 " << Virtual_Memory[i] << endl;
		}
		PCB.erase(it);
		break;
	}
    }
    return;
}




void Scheduler::RoundRobin(vector<Process>& PCB){
    vector<Process>::iterator it = PCB.begin();
		while(PCB.size() > 0){	// While the PCB isnt empty, run.
		//in_MM = false;

		if(it == PCB.end()){it = PCB.begin();} // If it reaches end of PCB, Loop back to start.

		switch(it->GetState()){


			case Process::New:	// Default state, Gets set to ready when running for the first time
				for(int i = 0; i < it->GetNumOfInstr(); i++){	// checks based on the number of instructions by
					for(int j = 0; j < NUM_OF_FRAMES; j++){		// looping through Memory to see if an open spot.
						if(Virtual_Memory[j] == -1){			// If yes, place in process number and set process to ready.
							Virtual_Memory[j] = it->GetNameNumber();
							it->SetPageTable(i,0,j);
							it->SetPageTable(i,1,1);
							enough_memory = true;
							break;
						}
						else{
							enough_memory = false;
						}
					}	
				}

				if(enough_memory){	// If there is enough virtual memory space, Set process to Ready.
					it->SetState(Process::Ready);
					it->printState();
					cout << endl << "--------------------- Virtual Memory ---------------------" << endl;
					cout << "Frame Number -- Process Number 	" << endl;
					for(int i = 0; i < NUM_OF_FRAMES;i++){
						cout << " " << i << ":	     	 " << Virtual_Memory[i] << endl;
					}
					break;
				}	// If there isnt enough, keep the process as new.
				it->SetState(Process::New);
				it->printState();
				break;


			case Process::Ready:	// Process is on CPU ready to move to running when scheduled.
				for(int i = 0;i < NUM_OF_FRAMES;i++){ // Loop through Main Memory to find page of current process.
					if(it->GetPageTable(it->GetPageNum(),0) == Main_Memory[i]){
						it->SetInMem(true);
						break;
					}
				}
				if(it->GetInMem() == false){ 
					for(int i = 0;i < NUM_OF_FRAMES;i++){ // If its not in memory, loop to find empty page.
						if(Main_Memory[i] == -1){
							Main_Memory[i] = it->GetPageTable(it->GetPageNum(),0);
							MM_full = false;
							break;
						}
						if(i == (NUM_OF_FRAMES - 1)){
							MM_full = true;
						}
					}
					if(MM_full == true){
						Main_Memory[full_mem_count] = it->GetPageTable(it->GetPageNum(),0);
						full_mem_count += 1;
					}
				}
				it->SetState(Process::Running);
				it->printState();
				if(MM_print == 1){
					cout << endl << "--------------------- Main Memory ---------------------" << endl;
					cout << "Frame Number -- Page Number 	" << endl;
					for(int i = 0; i < NUM_OF_FRAMES;i++){
						cout << " " << i << ":	     	 " << Main_Memory[i] << endl;
					}
				}
				break;


			case Process::Running:	// Ready -> Running. Stays if Calculate, moves to waiting if I/O or fork.
				if(it->GetInstructionType() == 1 && it->GetListSize() != 0)	// Checks if current instruction is I/O
				{
					it->SetState(Process::Waiting);
					it->printState();
					break;
				}
				
				if(it->GetInstructionType() == 3)	// Checks if Critical Section
				{
					if(critical == false){critical = true;}
					else{critical = false;}
					it->decrement_cycle();
					break;
				}

				if(it->decrement_cycle())	// If its Calculate, it checks if it can decrement a cicle. If so, run as normal
				{
					it->print();
					if(this->rr_count < (quantum - 1))
					{
						if(critical == false){this->rr_count++;}
					}
					else
					{
						it->SetState(Process::Ready);
						it->printState();
						if(critical == false){
							it++;
							this->rr_count = 0;
						}
					}
				}
				else if(it->GetListSize() != 0)	// if decrement_cycle == false, it checks if the Process is empty. If not, set state to ready.
				{
					it->SetState(Process::Ready);
					it->printState();
				}
				else // If the process is empty, terminate.
				{
					it->SetState(Process::Terminated);
					it->printState();
				}
				break;

			case Process::Waiting:	// State for I/O is called or if waiting on child process from fork.
				if((it->GetInstructionType()) == 0){it->SetState(Process::Ready);it->printState();break;}
				if(it->decrement_cycle())
				{
					it->print();
					if(this->rr_count < (quantum - 1))
					{
						if(critical == false){this->rr_count++;}
					}
					else
					{
						it->SetState(Process::Ready);
						it->printState();

						if(critical == false){
							it++;
							this->rr_count = 0;
						}
					}
				}
				else
				{
					it->SetState(Process::Ready);
					it->printState();
				}
				break;

			case Process::Terminated:	// State called right before deleted from PCB.
				for(int i = 0; i < NUM_OF_FRAMES; i++){ // Goes through Main Memory and clears
					for(int j = 0; j < NUM_OF_FRAMES; j++){ // Runs through page table
						if(Main_Memory[i] == it->GetPageTable(j,0) && it->GetPageTable(j,1) == 1){
							Main_Memory[i] = -1;
						}
					}
				}
				for(int i = 0; i < NUM_OF_FRAMES; i++){	// Goes through page table, clears memory, and then clears page table.
					if(it->GetPageTable(i,1) == 1){
						Virtual_Memory[it->GetPageTable(i,0)] = -1;
						it->SetPageTable(i,1,0);
					}
				}
				cout << endl << "--------------------- Virtual Memory ---------------------" << endl;
				cout << "Frame Number -- Process Number 	" << endl;
				for(int i = 0; i < NUM_OF_FRAMES;i++){
					cout << " " << i << ":	     	 " << Virtual_Memory[i] << endl;
				}
				PCB.erase(it);
				break;

		}
		}
}