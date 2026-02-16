CXX =g++
CXXFLAGS =-Wall -g


INCLUDES = xbtf.h
SOURCES = main.cpp xbtf.cpp xbtf_pack.cpp skin_unused.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = kodixbttool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(INCLUDES) $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) -lpthread -llzo2 -ljpeg -lpng -lgif -lsquish

clean:
	rm *.o $(EXECUTABLE); $(CTAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

