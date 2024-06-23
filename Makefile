CXX = g++
CXXFLAGS = -Wall -g 

lempelZivTest: test/lempelZivTest.cpp lempelZiv.o suffixTree.o
	$(CXX) $(CXXFLAGS) test/lempelZivTest.cpp lempelZiv.o suffixTree.o -o lempelZivTest.exe

lempelZiv.o: src/lempelZiv.cpp inc/lempelZiv.h
	$(CXX) $(CXXFLAGS) -c src/lempelZiv.cpp -o lempelZiv.o 

suffixTree.o: src/suffixTree.cpp inc/suffixTree.h
	$(CXX) $(CXXFLAGS) -c src/suffixTree.cpp -o suffixTree.o

.PHONY: clean
clean:
	rm -rf *.o
