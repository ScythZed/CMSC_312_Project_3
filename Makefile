CXX = g++	#turn off thie specific warning message - not needed for the g++ compiler

prog1 : main.o instruction.o
	$(CXX) main.o instruction.o -o prog1

main.o: main.cpp
	$(CXX) -c main.cpp

instruction.o: instruction.cpp
	$(CXX) -c instruction.cpp


clean:
	rm *.o prog*