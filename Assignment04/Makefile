vc:
	gcc -Wall -O2 -D_POSIX_C_SOURCE=199309L -I../utilities -Isrc -Idriver -o driver/vc_driver driver/vc_driver.c src/vc.c ../utilities/csr.c

pagerank:
	gcc -Wall -O2 -D_POSIX_C_SOURCE=199309L -I../utilities -Isrc -Idriver -o driver/pr_driver driver/pr_driver.c src/pr.c ../utilities/csr.c
CC = gcc
CFLAGS = -Wall -O2

UTIL = ../utilities
SRC = src
DRIVERDIR = driver

INCLUDES = -I$(UTIL) -I$(SRC) -I$(DRIVERDIR)

# ---- Vertex Coloring ----
VC_OBJS = csr.o vc.o vc_driver.o

vc: $(VC_OBJS)
	$(CC) $(CFLAGS) -o $(DRIVERDIR)/vc_driver $(VC_OBJS)

vc.o: $(SRC)/vc.c $(SRC)/vc.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC)/vc.c -o vc.o

vc_driver.o: $(DRIVERDIR)/vc_driver.c $(DRIVERDIR)/vc_driver.h $(SRC)/vc.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(DRIVERDIR)/vc_driver.c -o vc_driver.o

# ---- PageRank ----
PR_OBJS = csr.o pr.o pr_driver.o

pagerank: $(PR_OBJS)
	$(CC) $(CFLAGS) -o $(DRIVERDIR)/pr_driver $(PR_OBJS)

pr.o: $(SRC)/pr.c $(SRC)/pr.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC)/pr.c -o pr.o

pr_driver.o: $(DRIVERDIR)/pr_driver.c $(DRIVERDIR)/pr_driver.h $(SRC)/pr.h $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(DRIVERDIR)/pr_driver.c -o pr_driver.o

# ---- Shared ----
csr.o: $(UTIL)/csr.c $(UTIL)/csr.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(UTIL)/csr.c -o csr.o

clean:
	rm -f *.o driver/vc_driver driver/pr_driver