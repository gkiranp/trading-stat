################################################
#
# File: Makefile
#
# DEC 2016, By Kiran Puranik (gkiranp@gmail.com)
#
################################################

.DEFAULT_GOAL := all

OBJECTS = main.o utility.o parser.o msg_format.o
CPP = g++
LINK = g++
CPPFLAGS = 
TARGET = trade.exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(TARGET)

clean:
	rm $(OBJECTS) $(TARGET)

%.o: %.cpp
	$(CPP) -c -g -std=c++11 $(CPPFLAGS) -o $@ $<

