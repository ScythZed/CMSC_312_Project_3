// CMSC 312 Project - Part 1
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
using namespace std;
#include "instruction.h"

// Global Variables for Memory
extern const int MM_SIZE = 512; // 512MB (Main Memory storage)
extern const int VM_SIZE = 512; // 512MB (Virtual Memory storage)
extern const int FRAME_SIZE = 4; // 4MB (Size per frame)
extern const int NUM_OF_FRAMES = MM_SIZE / FRAME_SIZE; // total # of frames

//////// Instruction Functions ///////////////////////////

// Instruction()
// Summary: Default constructor
// Parameters: None
// Return: None 
Instruction::Instruction(){
	this->itype = 0;
	this->cycles = 0;
}

// Instruction()
// Summary: Secondary constructor
// Parameters:	int t - Instruction type
//				int c - # of cycles
// Return: None 
Instruction::Instruction(int t, int c){
	this->itype = t;
	this->cycles = c;
}

// ~Instruction()
// Summary: Default destructor
// Parameters: None
// Return: None 
Instruction::~Instruction(){}


// GetCycles()
// Summary: gets number of cycles of instruction
// Parameters:	None
// Return:	int number of cycles
int Instruction::GetCycles(){
	return this->cycles;
}

// GetItype()
// Summary: gets process state int value
// Parameters:	None
// Return:	int of State of Instruction
int Instruction::GetItype(){
	return this->itype;
}




// print()
// Summary: prints out the Instruction type and cycle count
// Parameters: None
// Return: None 
void Instruction::print(){
	switch(this->itype){
		case 0:
			cout << "CALCULATE ";
			break;
		case 1:
			cout << "I/O ";
			break;
		case 2:
			cout << "FORK ";
			break;
		case 3:
			cout << "---- CRITICAL SECTION ----" << endl;
			return;
			break;
	}
	cout << this->cycles << endl;
}

// decrement_cycle()
// Summary: reduces number of cycles by 1, unless the number of cycles is 0
// Parameters: None
// Return: None 
void Instruction::decrement_cycle(){
	if(this->cycles > 0)
		this->cycles -= 1;
}

// is_zero()
// Summary: checks if # of cycles is zero
// Parameters: None
// Return: Bool - True if zero 
bool Instruction::is_zero(){
	if(this->cycles > 0)
		return false;
	return true;
}


//////// Process Functions ////////////////////////////////

// Process()
// Summary: Default Process Constructor
Process::Process(){
	this->name = "None";
	this->state = Process::New;
	this->total_cycles = -1; // Gets set to 1 elsewhere. If it remains -1, then error.
	this->page_number = 0;
	for(int i = 0;i < NUM_OF_FRAMES;i++){
		page_table[i][0] = 0;
		page_table[i][1] = 0;
	}
	this->in_mem = false;
	return;
}

// SetName()
// Summary: sets process name
// Parameters:	name - string variable to set
// Return:	None
void Process::SetName(std::string name){
	this->name = name;
}

// GetName()
// Summary: gets process name
// Parameters:	None
// Return:	string of process name
string Process::GetName(){
	return this->name;
}

// SetNameNumber()
// Summary: sets process name
// Parameters:	num - int variable to set
// Return:	None
void Process::SetNameNumber(int num){
	this->name_number = num;
}

// GetNameNumber()
// Summary: sets process name
// Parameters:	None
// Return:	int of process name
int Process::GetNameNumber(){
	return this->name_number;
}

// SetState()
// Summary: sets process state
// Parameters:	input - Enum state variable to set
// Return:	None
void Process::SetState(Process::State input){
	this->state = input;
}

// GetState()
// Summary: sets process state
// Parameters:	None
// Return:	Enum State of Process
Process::State Process::GetState(){
	return this->state;
}




// printState()
// Summary:  process state
// Parameters:	None
// Return:	String equivialnt of state
void Process::printState(){
	switch(this->state){
		case Process::New:
			cout << endl << this->GetName() << " --- New" << endl << endl; // State Output
			break;
		case Process::Ready:
			cout << endl << this->GetName() << " --- Ready" << endl << endl; // State Output
			break;
		case Process::Running:
			cout << endl << this->GetName() << " --- Running" << endl << endl; // State Output
			break;
		case Process::Waiting:
			cout << endl << this->GetName() << " --- Waiting" << endl << endl; // State Output
			break;
		case Process::Terminated:
			cout << endl << this->GetName() << " --- Terminated" << endl << endl; // State Output
			break;

	}
}

// load()
// Summary: loads Instruction objects into process vector
// Parameters:	fstream input file.
// Return:	Returns false if eof(infile), else true
bool Process::load(fstream& infile){
	// Temp Variables used to create instruction object
	char chartype;
	int lowcycles, upcycles, cyclerange;
	int rand_cycles, type;

	infile >> chartype;
	if(chartype == 'F'){
		List.push_back(Instruction(2,0));
		return true;
	}
	if(chartype == '!'){
		List.push_back(Instruction(3,0));
		return true;
	}
	infile >> lowcycles;
	infile >> upcycles;

	// Calculate range of cycles
	cyclerange = upcycles - lowcycles;

	// Assigns Instruction type based on char from file.
	switch(chartype){
		case('C'):
			type = 0;
			break;
		case('I'):
			type = 1;
			break;
		case('F'):
			type = 2;
			break;
		}

	// Creates and pushes back objects in the vector
	rand_cycles = ((rand() % cyclerange) + lowcycles);
	List.push_back(Instruction(type,rand_cycles));

	if(infile.eof()){
		return false;
	}
	else{return true;}
}


// print()
// Summary: prints out the process name and all of the instructions
// Parameters: None
// Return: None 
void Process::print(){
	cout << "------------------------- " << this->GetName() << " -------------------------\n";
	cout << "------------------------   " << "CPU #" << this->CPU_Num << "    ------------------------\n";
	for (vector<Instruction>::iterator it = List.begin(); it != List.end(); ++it)
    	it->print();
    cout << "-------------------------------------------------------------\n\n";
}


// decrement_cycle()
// Summary:	decrements a cycle count from the top of the instructions list.
//			if the instruction element is 0, it removes it from the list
//			and decrements the next.
// Parameters: None
// Return: Returns false if instructions list is empty, else true.
bool Process::decrement_cycle(){
	vector<Instruction>::iterator it = List.begin();
	while(it != List.end()){
		//it->decrement_cycle();
		if(it->is_zero() == false){
    		it->decrement_cycle();
    		return true;
	    }
    	else{
    		List.erase(it);
    		return false;
	    }
	}
	return false;

}


// GetInstructionType()
// Summary: prints out the process name and all of the instructions
// Parameters: None
// Return: Int of current Instruction Type.
int Process::GetInstructionType(){
	vector<Instruction>::iterator it = List.begin();
	return it->GetItype();
}

// GetListSize()
// Summary: Returns number of instructions in the process
// Parameters: None
// Return: Int number of intstructions on the Process list.
int Process::GetListSize(){
	return this->List.size();
	}

// SetCycleTotal()
// Summary: Gets number of cycles in the process
// Parameters: None
// Return: None
void Process::SetCycleTotal(){
	int cycle_number;
	vector<Instruction>::iterator it = List.begin();
	while(it != List.end()){
		cycle_number += it->GetCycles();
		it++;
	}
	this->total_cycles = cycle_number;
}

// GetCycleTotal()
// Summary: Sets number of cycles in the process
// Parameters: None
// Return: Int number of cycles on the Process list.
int Process::GetCycleTotal(){
	return this->total_cycles;
}



// GetNumOfInstr()
// Summary: Returns number of intructions that preform cycles.
// Parameters: none
// Return: int
int Process::GetNumOfInstr(){
	int count = 0;
	vector<Instruction>::iterator it = List.begin();
	while(it != List.end()){
		if(it->GetItype() == 0 || it->GetItype() == 1){
			count += 1;
		}
		it++;
	}
	return count;
}

// SetPageSize()
// Summary: Sets page_size based on NumOfInstr()
// Parameters: none
// Return: none
void Process::SetPageSize(){
	this->page_size = this->GetNumOfInstr();
	return;
}

// GetPageSize()
// Summary: Get page_size based on set value
// Parameters: none
// Return: Int
int Process::GetPageSize(){
	return this->page_size;
}

// GetPageTable()
// Summary: Gets a value in the PageTable based on parameters
// Parameters: x - Row, Y - Colloum
// Return: data at location
int Process::GetPageTable(int x, int y){
	return this->page_table[x][y];
}

// SetPageTable()
// Summary: Sets a value in the PageTable based on parameters
// Parameters: x - Row, Y - Colloum, data - data to set.
// Return: none
void Process::SetPageTable(int x, int y, int data){
	this->page_table[x][y] = data;
	return;
}


// PageTablePrint()
// Summary: prints out the page table of a process
// Parameters: None
// Return: None 
void Process::PageTablePrint(){
	cout << "------------- " << this->GetName() << " ~ Page Table -------------\n";
	for(int i = 0;i < NUM_OF_FRAMES;i++){
		if(this->page_table[i][1] == 1){
			cout << i << ") ";
			cout << "Frame Number: " << page_table[i][0];
			cout << " ~ Validity Bit: " << page_table[i][1] << endl;
		}
	}
	cout << "--------------------------------------------------\n";
	return;
}




// GetPageNum()
// Summary: Gets page number based on set value
// Parameters: none
// Return: int value of the page table number of the instruction
// 			we are currently at.
int Process::GetPageNum(){
	return (this->GetPageSize() - this->GetNumOfInstr());
}


// GetInMem()
// Summary: Returns if Process is in memory
// Parameters: none
// Return: Returns if Process is in memory
bool Process::GetInMem(){
	return this->in_mem;
}


// SetInMem()
// Summary: Sets In Memory data member
// Parameters: bool
// Return: none.
void Process::SetInMem(bool InMem){
	this->in_mem = InMem;
	return;
}


// SetCPUNum()
// Summary: Sets CPU number data member
// Parameters: int
// Return: none.
void Process::SetCPUNum(int num){
	this->CPU_Num = num;
	return;
}


// SetThreadNum()
// Summary: Sets the thread id of the thread that currently running the Process.
// Parameters: bool
// Return: none.
void Process::SetThreadNum(int thread){
	this->thread_number = thread;
	return;
}

// GetInMem()
// Summary: Gets the thread id number.
// Parameters: none
// Return: process_active
int Process::GetThreadNum(){
	return this->thread_number;
}