# Voro++ makefile
#
# Author : Chris H. Rycroft (LBL / UC Berkeley)
# Email  : chr@alum.mit.edu
# Date   : August 30th 2011

# Load the common configuration file
include ../../config.mk

# List of executables
EXECUTABLES=radical_2D

# Makefile rules
all: $(EXECUTABLES)

radical_2D: radical_2D.cc
	$(CXX) $(CFLAGS) $(E_INC) $(E_LIB) -o radical_2D radical_2D.cc -lvoro++

clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean
