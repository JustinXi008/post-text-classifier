CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2

.PHONY: all clean

all: classifier

classifier: classifier.cpp csv_reader.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) classifier.cpp $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f classifier
	rm -rf classifier.dSYM
