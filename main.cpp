// CMSC 312 Project - Part 2
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;
#include "instruction.h"


// PCB Cycle size sort (Shortest first)
bool cycle_lt(Process x, Process y){
	int x_size,y_size;
	x_size = x.GetCycleTotal();
	y_size = y.GetCycleTotal();
	return (x_size < y_size);
	
}

int main(int argc, char *argv[]){

	////	Variables	//////////////////////////////////////

	// fstream variables for file intake
	fstream infile;
	fstream fork_file;
	string infilestr = "input.txt";

	// Process Vector
	vector<Process> PCB;
	int process_cout = 2;
	string process_name = "Process ";

	// Scheduling
	int scheduling;	// 0 = Round Robin, 1 = Shortest First
	int quantum = 5;
	int rr_count = 0;
	bool critical = false;
	bool forked = false;

	// Memory
	int Main_Memory[NUM_OF_FRAMES];
	int Virtual_Memory[NUM_OF_FRAMES];
	bool enough_memory = true;
	bool in_MM = false;
	bool MM_full = false;
	int full_mem_count = 0;
	int MM_print = 0;

	//////////////////////////////////////////////////////////


	// Command line argument intake and checking
	if(argc > 2) // Too many arguments
	{
		cout << "\nERROR: Too many arguments.\n";
		cout << "Usage: prog1 <infile name>\n";
		return -1;
	}
	else if(argc == 2)
	{
		infilestr = argv[1];
		assert(process_cout > 0);
	}
	else // Too few arguments
	{
		cout << "\nERROR: Too many arguments.\n";
		cout << "Usage: prog1 <infile name>\n";
		return -1;
	}

	// Empties Main Memory and Virtual Memory
	for(int i = 0;i < NUM_OF_FRAMES;i++){
		Main_Memory[i] = -1;
		Virtual_Memory[i] = -1;
	}

	// Intakes scheduling info
	cout << "Enter number of process: ";
	cin >> process_cout;
	assert(process_cout >= 0);
	cout << endl;
	cout << "Enter type of scheduler (0 = Round Robin, 1 = Shortest First): ";
	cin >> scheduling;
	assert(scheduling == 0 || scheduling == 1);
	cout << "Print Main Memory during Running state? (0 = No, 1 = Yes): ";
	cin >> MM_print;
	assert(MM_print == 0 || MM_print == 1);


	// Calls srand to make random number generating work in function.
	srand(time(NULL));


	// Loops and creates Processes to put in PCB.
	for(int i=0;i<process_cout;i++)
	{
		Process Loop;

		// Opens intake file
		infile.open(infilestr,fstream::in);
		if(infile.is_open() == false){
			cout << "ERROR: file " << infilestr << " can not be opened!" << endl;
			return -1;
		}

		// Loads all instructions from file
		while(Loop.load(infile)){}
		// Sets process name, state, and pushes onto PCB
		Loop.SetName(process_name + to_string(i));
		Loop.SetNameNumber(i);
		Loop.SetState(Process::New);
		Loop.SetCycleTotal();
		Loop.SetPageSize();
		PCB.push_back(Loop);
		infile.close();
	}

	
	fork_file.open("fork.txt",fstream::in);
	if(fork_file.is_open() == false){
		cout << "ERROR: file " << "fork.txt" << " can not be opened!" << endl;
		return -1;
	}
	

	// Prints processes in the PCB (Also sets PCB Process to ready)
	for (vector<Process>::iterator it = PCB.begin(); it != PCB.end(); ++it)
	{
		it->print();
		it->SetState(Process::New);
	}


	if(scheduling == 0){	// Round Robin (/w Quantum) ***************************************************************************************************
		vector<Process>::iterator it = PCB.begin();
		while(PCB.size() > 0){	// While the PCB isnt empty, run.
		in_MM = false;

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
						in_MM = true;
						break;
					}
				}
				if(in_MM == false){ 
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

				if(it->GetInstructionType() == 2 && it->GetListSize() != 0)	// Checks if current instruction is fork
				{
					// Creates child process
					it->decrement_cycle();
					Process PFork;
					PFork.SetName(it->GetName() + "'s Child");
					while(PFork.load(fork_file)){}
					PCB.push_back(PFork);
					it = PCB.begin();
					forked = true;
					PFork.print();
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
					if(rr_count < (quantum - 1))
					{
						if(critical == false){rr_count++;}
					}
					else
					{
						it->SetState(Process::Ready);
						it->printState();
						if(critical == false){
							it++;
							rr_count = 0;
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
					if(rr_count < (quantum - 1))
					{
						if(critical == false){rr_count++;}
					}
					else
					{
						it->SetState(Process::Ready);
						it->printState();

						if(critical == false){
							it++;
							rr_count = 0;
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
	else{	// Shortest First **********************************************************************************************************************************
		sort(PCB.begin(),PCB.end(),cycle_lt);	// Sorts PCB

		vector<Process>::iterator it = PCB.begin();
		while(PCB.size() > 0){	// While the PCB isnt empty, run.
		in_MM = false;

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
						in_MM = true;
						break;
					}
				}
				if(in_MM == false){ 
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

				if(it->GetInstructionType() == 2)	// Checks if Fork Section
				{
					it->decrement_cycle();
					Process PFork;
					PFork.SetName("Forked");
					while(PFork.load(fork_file)){}
					PCB.push_back(PFork);
					cout << PCB.size() << endl;
					forked = true;
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

		
	}
	// ***********************************************************************************************************************************************

	cout << endl << "--------------------- Main Memory ---------------------" << endl;
	cout << "Frame Number -- Page Number 	" << endl;
	for(int i = 0; i < NUM_OF_FRAMES;i++){
		cout << " " << i << ":	     	 " << Main_Memory[i] << endl;
	}
	


}

