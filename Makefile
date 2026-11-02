CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2

.PHONY: all clean

all: classifier

classifier: classifier.cpp csvstream.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) classifier.cpp $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f classifier
	rm -rf classifier.dSYM
