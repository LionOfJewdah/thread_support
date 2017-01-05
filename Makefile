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
TARGET        = elHol_rloWrd.out

####### Files

SOURCES     = elHol_rloWrd.cpp

HEADERS		  =

TEST_SOURCES =
EXECS		  	= elHol_rloWrd.out

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: $(TARGET)
	-@$(BIN_DIR)/$(TARGET)

elHol_rloWrd.out: elHol_rloWrd.o $(BIN_DIR)/.dirstamp
	$(LINK) $< $(THREADING) $(LFLAGS) $(CLARGS) -o $(BIN_DIR)/$@

clean:
	rm -f $(EXECS)

$(BIN_DIR)/.dirstamp:
	-@mkdir -p $(BIN_DIR)
	-@touch $@

$(TEST_DIR)/.dirstamp:
	-@mkdir -p $(TEST_DIR)
	-@touch $@

demonstrate: $(TARGET)
	-@for var in `seq 1 20`; do \
			$(BIN_DIR)/$(TARGET); \
	done

concurrency: demonstrate
