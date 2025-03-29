# Ben Leber
# 3/28/25
# Makefile for producer-consumer problem

# uses g++ compiler with -pthread and -lrt flags 
CXX = g++
CXXFLAGS = -pthread -lrt
TARGETS = producer consumer

all: $(TARGETS)

producer: producer.cpp
	$(CXX) producer.cpp $(CXXFLAGS) -o producer

consumer: consumer.cpp
	$(CXX) consumer.cpp $(CXXFLAGS) -o consumer

clean:
	rm -f $(TARGETS)
