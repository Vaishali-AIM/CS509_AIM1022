CC = gcc
CFLAGS = -Wall -O2 -D_POSIX_C_SOURCE=200809L

UTIL = ../utilities
SRC = src
DRIVERDIR = driver

INCLUDES = -I$(UTIL) -I$(SRC)

OBJS = csr.o mst.o driver.o

mst_driver: $(OBJS)
	$(CC) $(CFLAGS) -o mst_driver $(OBJS)

csr.o: $(UTIL)/csr.c $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(UTIL)/csr.c -o csr.o

mst.o: $(SRC)/mst.c $(SRC)/mst.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC)/mst.c -o mst.o

driver.o: $(DRIVERDIR)/driver.c $(SRC)/mst.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(DRIVERDIR)/driver.c -o driver.o

clean:
	rm -f *.o mst_driver