.POSIX:
.SUFFIXES:

default: silver gold

silver: 10.cpp
	$(CXX) -o $@ 10.cpp -DSILVER $(CXXFLAGS)

gold: 10.cpp
	$(CXX) -o $@ 10.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
