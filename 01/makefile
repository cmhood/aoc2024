.POSIX:
.SUFFIXES:

default: silver gold

silver: 01.cpp
	$(CXX) -o $@ 01.cpp -DSILVER $(CXXFLAGS)

gold: 01.cpp
	$(CXX) -o $@ 01.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
