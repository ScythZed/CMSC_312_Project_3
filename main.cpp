// CMSC 312 Project - Part 2
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <thread>
#include <mutex>


//#include "instruction.h"
#include "scheduler.h"
using namespace std;

int Main_Memory[128];
int Virtual_Memory[128];
bool critical = false;
bool enough_memory = true;
bool MM_full = false;
int full_mem_count = 0;
int MM_print = 0;

std::mutex mtx;

// Thread Testing
void thread_func(std::string temp = "Goodbye!\n"){
	mtx.lock();
	cout << "Thread ID:" << this_thread::get_id() << endl;
	mtx.unlock();
	return;
}



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
	vector<Process> PCB1;	// PCB for CPU 1
	vector<Process> PCB2;	// PCB for CPU 2
	int process_cout = 2;
	string process_name = "Process ";

	// Scheduling
	int scheduling1;	// 0 = Round Robin, 1 = Shortest First
	int scheduling2;	// 0 = Round Robin, 1 = Shortest First
	int quantum = 5;
	int rr_count = 0;
	bool critical = false;
	bool forked = false;

	// Memory - All defined as Global at top of file. (Saved for Legacy)
	// int Main_Memory[NUM_OF_FRAMES];
	// int Virtual_Memory[NUM_OF_FRAMES];
	// bool enough_memory = true;
	// bool in_MM = false;
	// bool MM_full = false;
	// int full_mem_count = 0;
	// int MM_print = 0;

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
		cout << "\nERROR: Too few arguments.\n";
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
	cout << "Enter type of scheduler for CPU #1 (0 = Round Robin, 1 = Shortest First): ";
	cin >> scheduling1;
	//assert(scheduling1 == 0 || scheduling1 == 1);
	cout << "Enter type of scheduler for CPU #2 (0 = Round Robin, 1 = Shortest First): ";
	cin >> scheduling2;
	assert(scheduling2 == 0 || scheduling2 == 1);
	cout << "Print Main Memory during Running state? (0 = No, 1 = Yes): ";
	cin >> MM_print;
	assert(MM_print == 0 || MM_print == 1);


	// Calls srand to make random number generating work in function.
	srand(time(NULL));


	// Loops and creates Processes to put in PCB1
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
		// Sets process name, state, and pushes onto PCB1
		Loop.SetName(process_name + to_string(i));
		Loop.SetNameNumber(i);
		Loop.SetState(Process::New);
		Loop.SetCycleTotal();
		Loop.SetPageSize();
		Loop.SetCPUNum(1);
		PCB1.push_back(Loop);
		infile.close();
	}
	// Again, but for PCB2.
	for(int i=0;i<process_cout;i++)
	{
		Process Loop2;

		// Opens intake file
		infile.open(infilestr,fstream::in);
		if(infile.is_open() == false){
			cout << "ERROR: file " << infilestr << " can not be opened!" << endl;
			return -1;
		}

		// Loads all instructions from file
		while(Loop2.load(infile)){}
		// Sets process name, state, and pushes onto PCB1
		Loop2.SetName(process_name + to_string((i + process_cout)));
		Loop2.SetNameNumber(i + process_cout);
		Loop2.SetState(Process::New);
		Loop2.SetCycleTotal();
		Loop2.SetPageSize();
		Loop2.SetCPUNum(2);
		PCB2.push_back(Loop2);
		infile.close();
	}
	
	fork_file.open("fork.txt",fstream::in);
	if(fork_file.is_open() == false){
		cout << "ERROR: file " << "fork.txt" << " can not be opened!" << endl;
		return -1;
	}
	
	// Prints processes in the PCB (Also sets PCB Process to ready)
	for (vector<Process>::iterator it = PCB1.begin(); it != PCB1.end(); ++it)
	{
		it->print();
		it->SetState(Process::New);
	}
	for (vector<Process>::iterator it = PCB2.begin(); it != PCB2.end(); ++it)
	{
		it->print();
		it->SetState(Process::New);
	}
	

	// Scheduling ***************************************************************************************************
	Scheduler scheduled_thread1;	// Thread 1 - CPU 1
	Scheduler scheduled_thread2;	// Thread 2 - CPU 1
	Scheduler scheduled_thread3;	// Thread 3 - CPU 2
	Scheduler scheduled_thread4;	// Thread 4 - CPU 2
	if(scheduling1 == 0 && scheduling2 == 0){	//	Round Robin - CPU 1 & Round Robin - CPU 2
		thread thread1_cpu1(&Scheduler::RoundRobin,&scheduled_thread1,std::ref(PCB1));
		thread thread2_cpu1(&Scheduler::RoundRobin,&scheduled_thread2,std::ref(PCB1));
		thread thread3_cpu2(&Scheduler::RoundRobin,&scheduled_thread1,std::ref(PCB2));
		thread thread4_cpu2(&Scheduler::RoundRobin,&scheduled_thread2,std::ref(PCB2));
		thread1_cpu1.join();
		thread2_cpu1.join();
		thread3_cpu2.join();
		thread4_cpu2.join();
	}
	else if (scheduling1 == 1 && scheduling2 == 0){	// Shortest First - CPU 1 & Round Robin - CPU 2
		sort(PCB1.begin(),PCB1.end(),cycle_lt);
		thread thread1_cpu1(&Scheduler::ShortestFirst,&scheduled_thread1,std::ref(PCB1));
		thread thread2_cpu1(&Scheduler::ShortestFirst,&scheduled_thread2,std::ref(PCB1));
		thread thread3_cpu2(&Scheduler::RoundRobin,&scheduled_thread1,std::ref(PCB2));
		thread thread4_cpu2(&Scheduler::RoundRobin,&scheduled_thread2,std::ref(PCB2));
		thread1_cpu1.join();
		thread2_cpu1.join();
		thread3_cpu2.join();
		thread4_cpu2.join();
	}
	else if(scheduling1 == 0 && scheduling2 == 1){	//	Round Robin - CPU 1 & Shortest First - CPU 2
		sort(PCB2.begin(),PCB2.end(),cycle_lt);
		thread thread1_cpu1(&Scheduler::RoundRobin,&scheduled_thread1,std::ref(PCB1));
		thread thread2_cpu1(&Scheduler::RoundRobin,&scheduled_thread2,std::ref(PCB1));
		thread thread3_cpu2(&Scheduler::RoundRobin,&scheduled_thread1,std::ref(PCB2));
		thread thread4_cpu2(&Scheduler::RoundRobin,&scheduled_thread2,std::ref(PCB2));
		thread1_cpu1.join();
		thread2_cpu1.join();
		thread3_cpu2.join();
		thread4_cpu2.join();
	}
	else if(scheduling1 == 1 && scheduling2 == 1){	//	Shortest First - CPU 1 & Shortest First - CPU 2
		sort(PCB1.begin(),PCB1.end(),cycle_lt);
		sort(PCB2.begin(),PCB2.end(),cycle_lt);
		thread thread1_cpu1(&Scheduler::ShortestFirst,&scheduled_thread1,std::ref(PCB1));
		thread thread2_cpu1(&Scheduler::ShortestFirst,&scheduled_thread2,std::ref(PCB1));
		thread thread3_cpu2(&Scheduler::ShortestFirst,&scheduled_thread1,std::ref(PCB2));
		thread thread4_cpu2(&Scheduler::ShortestFirst,&scheduled_thread2,std::ref(PCB2));
		thread1_cpu1.join();
		thread2_cpu1.join();
		thread3_cpu2.join();
		thread4_cpu2.join();
	}

	// **************************************************************************************************************



	// Printing Main Memory
	cout << endl << "--------------------- Main Memory ---------------------" << endl;
	cout << "Frame Number -- Page Number 	" << endl;
	for(int i = 0; i < NUM_OF_FRAMES;i++){
		cout << " " << i << ":	     	 " << Main_Memory[i] << endl;
	}


}

