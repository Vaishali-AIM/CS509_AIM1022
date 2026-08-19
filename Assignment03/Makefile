CC = gcc
CXX = g++
CFLAGS = -Wall -O2
CXXFLAGS = -Wall -O2 -std=c++17

UTIL = ../utilities
MST = MST
DRIVER = Driver

INCLUDES = -I$(UTIL) -I$(MST)

OBJS = csr.o mst.o driver.o

run: $(OBJS)
	$(CXX) $(CXXFLAGS) -o run $(OBJS)

csr.o: $(UTIL)/csr.c $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(UTIL)/csr.c -o csr.o

mst.o: $(MST)/mst.cpp $(MST)/mst.h $(UTIL)/csr.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(MST)/mst.cpp -o mst.o

driver.o: $(DRIVER)/driver.cpp $(MST)/mst.h $(UTIL)/csr.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(DRIVER)/driver.cpp -o driver.o

clean:
	rm -f *.o run
