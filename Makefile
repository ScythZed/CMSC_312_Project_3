CXX = g++	#turn off thie specific warning message - not needed for the g++ compiler

prog1 : main.o instruction.o scheduler.o
	$(CXX) -std=c++11 main.o instruction.o scheduler.o -o prog1

main.o: main.cpp
	$(CXX) -c -std=c++11 main.cpp

instruction.o: instruction.cpp instruction.h
	$(CXX) -c -std=c++11 instruction.cpp

scheduler.o: scheduler.cpp scheduler.h
	$(CXX) -c -std=c++11 scheduler.cpp

clean:
	rm *.o prog*