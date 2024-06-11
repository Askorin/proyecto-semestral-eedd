CXX = g++
CXXFLAGS = -Wall -g 

lempelziv: src/lempelZiv.cpp suffixTree.o
	$(CXX) $(CXXFLAGS) src/lempelZiv.cpp suffixTree.o -o lempelziv.exe

suffixTree.o: src/suffixTree.cpp inc/suffixTree.h
	$(CXX) $(CXXFLAGS) -c src/suffixTree.cpp -o suffixTree.o


.PHONY: clean
clean:
	rm -rf *.o

