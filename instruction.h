// CMSC 312 Project - Part 1
#include<queue>
#include<vector>
#include<fstream>

// Global Variables for Memory
extern const int MM_SIZE; // 512MB (Main Memory storage)
extern const int VM_SIZE; // 512MB (Virtual Memory storage)
extern const int FRAME_SIZE; // 4MB (Size per frame)
extern const int NUM_OF_FRAMES; // total # of frames


class Instruction{

	protected:
		int cycles; // Number of cycles for the instruction
		int itype; 	// 0 = CALCULATE, 1 = I/O, 2 = FORK, 3 = Critical Section

	public:
		Instruction();
		Instruction(int t, int c);
		~Instruction();

		int GetCycles();
		int GetItype();

		void print();
		void decrement_cycle();
		bool is_zero();

	};

class Process: public Instruction{
	public:
		enum State { New, Ready, Running, Waiting, Terminated };

		Process();

		void SetName(std::string name);
		std::string GetName();

		void SetNameNumber(int num);
		int GetNameNumber();

		void SetState(State input);
		State GetState();

		void printState();

		bool load(fstream& infile);
		void print();
		bool decrement_cycle();

		int GetInstructionType();
		int GetListSize();

		void SetCycleTotal();
		int GetCycleTotal();

		int GetNumOfInstr();
		void SetPageSize();
		int GetPageSize();

		void SetPageTable(int x, int y, int data);
		int GetPageTable(int x, int y);
		void PageTablePrint();

		int GetPageNum();

	private:
		std::vector<Instruction> List; // Vector that holds all the process's instructions 
		std::string name; // Name of the Process instance
		int name_number; // Number at the end of the process
		State state; // Enum for state machine
		int total_cycles; // Number of cycles for the process

		int page_table[128][2]; // [NUM_OF_FRAMES] - First row is for frame number. Second row is validity bit(1 or 0).
		int page_size; // Number of instructions in the page.
		int page_number;

};
