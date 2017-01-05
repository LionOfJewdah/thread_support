#############################################################################
# Makefile for building: AVL Trees
# David Silverstone
# Nov. 17, 2016
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
STD			  		= -std=c++11
CLARGS		  	=
DEFINES       = $(CLARGS)
CFLAGS        = -m64 -pipe -O2 -g -Wall -W $(DEFINES)
CXXFLAGS      = -m64 -pipe -O2 $(STD) -g -Wall -W $(DEFINES)
LINK          = g++
LFLAGS        = -m64 -Wl,-O1
LIBS		  		= -L/usr/lib/x86_x64-linux-gnu -lpthread $(THREADING)
GTEST_LL	  	= -I /usr/include/gtest/ -l gtest -l gtest_main -pthread
AR            = ar cqs
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
THREADING     = -pthread


####### Output directory

BIN_DIR   	  = .
TEST_DIR	  	= tests
OUT_DIR				= output
RES_DIR				= resources
TARGET        = elHol_rloWrd.out

####### Files

SOURCES       = elHol_rloWrd.cpp thread_stack.cpp thread_queue.cpp

HEADERS		    = structs_fwd.hpp thread_queue.hpp thread_stack.hpp

TEXT_FILES    = $(RES_DIR)/kesha.txt $(RES_DIR)/row_your_boat.txt

TEST_SOURCES  =
EXECS		  		= elHol_rloWrd.out thread_queue.out thread_stack.out

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(THREADING) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

.o.out:
	$(LINK) $< $(THREADING) $(LFLAGS) -o $(BIN_DIR)/$@
####### Build rules

all: $(TARGET)
	-@$(BIN_DIR)/$(TARGET)

thread_queue.o: thread_queue.cpp structs_fwd.hpp thread_queue.hpp
thread_stack.o: thread_stack.cpp structs_fwd.hpp thread_stack.hpp
thread_priority_queue.o: thread_priority_queue.cpp structs_fwd.hpp \
		thread_priority_queue.hpp

solve_equations.o: solve_equations.cpp structs_fwd.hpp thread_queue.hpp
		$(CXX) -c $(CXXFLAGS) -std=c++14 $(THREADING) $(INCPATH) -o "$@" "$<"

elHol_rloWrd.out: elHol_rloWrd.o $(BIN_DIR)/.dirstamp
thread_queue.out: thread_queue.o $(BIN_DIR)/.dirstamp #$(TEXT_FILES)
thread_stack.out: thread_stack.o $(BIN_DIR)/.dirstamp #$(TEXT_FILES)
thread_priority_queue.out: thread_priority_queue.o $(BIN_DIR)/.dirstamp
	$(LINK) $< $(THREADING) $(LFLAGS) $(CLARGS) -o $(BIN_DIR)/$@

clean:
	rm -f $(EXECS)

$(BIN_DIR)/.dirstamp:
	-@mkdir -p $(BIN_DIR)
	-@touch $@

$(TEST_DIR)/.dirstamp:
	-@mkdir -p $(TEST_DIR)
	-@touch $@

$(RES_DIR)/.dirstamp:
	-@mkdir -p $(RES_DIR)
	-@touch $@

$(OUT_DIR)/.dirstamp:
	-@mkdir -p $(OUT_DIR)
	-@touch $@

demonstrate: elHol_rloWrd.out
	-@for var in `seq 1 20`; do \
			$(BIN_DIR)/$(TARGET); \
	done

concurrency: demonstrate

Equations: generate_math.out
	-@$(BIN_DIR)/$< $(CLARGS)
Solve: 		 solve_equations.out $(OUT_DIR)/.dirstamp
	-@$(BIN_DIR)/$< $(CLARGS)
