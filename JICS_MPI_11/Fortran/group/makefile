#--------------------------------------------------------
# makefile for the group.f lab exercise
#--------------------------------------------------------

#--------------------------------------------------------
# Platform specific MPI compilers 
#--------------------------------------------------------

CC = ftn

#--------------------------------------------------------
# The filename to be used for the executable
#--------------------------------------------------------

EXECUTABLE = group

#--------------------------------------------------------
# The source code to be compiled
#--------------------------------------------------------

SRC = group.f

#--------------------------------------------------------
# The compilation rules. Notice that 'TAB' symbol is used
#--------------------------------------------------------
$(EXECUTABLE): $(SRC) 
	$(CC) -o $@ $(SRC)
