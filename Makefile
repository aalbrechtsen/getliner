CXX = g++
LIBS = -lz
FLAGS = -O3 


getliner: getliners.cpp 
	$(CXX) getliners.cpp $(FLAGS) $(LIBS) -o getliner


clean:
	rm getliner
