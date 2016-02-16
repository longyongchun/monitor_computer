src_dir  = $(shell pwd)
src_file = $(shell find ${src_dir} -name "*.cpp")
objects  = $(src_file:%.cpp=%.o)

CXX = g++
CXXFLAGS = -g -std=c++11 -pthread -lboost_thread -lboost_system

main : $(objects)
	$(CXX) $(objects) -omain $(CXXFLAGS)


.PHONY : clean

clean:
	-rm main $(objects)


