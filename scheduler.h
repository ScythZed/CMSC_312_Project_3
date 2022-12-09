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
        int scheduling_type;
        int quantum;
        int rr_count;
        static std::mutex mtx;

    public:
        Scheduler();    // Default Construtor
        void print();   // Print function
        void ShortestFirst(std::vector<Process>& PCB); // Shortest First
        void RoundRobin(std::vector<Process>& PCB);  // Round Robin

        void threadprint(std::vector<Process>& PCB);
    };
