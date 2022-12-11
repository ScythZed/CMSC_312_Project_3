// CMSC 312 Project - Part 3
#include <queue>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include "instruction.h"

extern int Main_Memory[128];
extern int Virtual_Memory[128];
extern bool critical;
extern bool enough_memory;
extern bool MM_full;
extern int full_mem_count;
extern int MM_print;

class Scheduler{
    protected:
        int scheduling_type;    // RR or SF
        int quantum;    // Set by default to 5 in construtor
        int rr_count;   // Interated apon within RR scheduler
        static std::mutex mtx;  // Global Mutex accross threads

    public:
        Scheduler();    // Default Construtor
        void print();   // Print function
        void ShortestFirst(std::vector<Process>& PCB); // Shortest First
        void RoundRobin(std::vector<Process>& PCB);  // Round Robin

    };
