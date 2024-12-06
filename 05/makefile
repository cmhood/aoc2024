.POSIX:
.SUFFIXES:

default: silver gold

silver: 05.cpp
	$(CXX) -o $@ 05.cpp -DSILVER $(CXXFLAGS)

gold: 05.cpp
	$(CXX) -o $@ 05.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
