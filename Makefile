# Voro++ makefile
#
# Author : Chris H. Rycroft (LBL / UC Berkeley)
# Email  : chr@alum.mit.edu
# Date   : August 30th 2011

# Load the common configuration file
include ../../config.mk

# name of executable
EXECUTABLES=radical_2D

# Makefile rules
all: radical_2D.cc mpp.c
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o radical_2D radical_2D.cc mpp.c -lvoro++ -lm

rad_2D: radical_2D.cc
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o radical_2D.o -c radical_2D.cc -lvoro++

mpp: mpp.c
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o mpp.o -c mpp.c -lm

# If any object file has changed, re-link the binary object
# should be automatic, but I don't think it is...
# work on this
${EXECUTABLES}: 
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o radical_2D radical_2D.o mpp.o -lvoro++ -lm

clean:
	rm -f $(EXECUTABLES) *~ *.o

.PHONY: all clean
