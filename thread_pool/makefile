src_dir = $(shell pwd)
src_file = $(shell find ${src_dir} -name "*.cpp")
objects = $(src_file:%.cpp=%.o)

CXX = g++
CXXFLAGS = -std=c++11 -g -L/usr/local/lib/ -pthread -lboost_thread -lboost_system  

main : $(objects)
	$(CXX) $(CXXFLAGS) -omain $(objects)

.PHONY : clean
clean : 
	-rm main $(objects)

