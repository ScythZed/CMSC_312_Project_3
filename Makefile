CXX = g++	#turn off thie specific warning message - not needed for the g++ compiler

prog1 : main.o instruction.o scheduler.o
	$(CXX) main.o instruction.o scheduler.o -o prog1

main.o: main.cpp
	$(CXX) -c main.cpp

instruction.o: instruction.cpp instruction.h
	$(CXX) -c instruction.cpp

scheduler.o: scheduler.cpp scheduler.h
	$(CXX) -c scheduler.cpp

clean:
	rm *.o prog*