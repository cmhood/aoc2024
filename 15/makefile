.POSIX:
.SUFFIXES:

default: silver gold

silver: 15.cpp
	$(CXX) -o $@ 15.cpp -DSILVER $(CXXFLAGS)

gold: 15.cpp
	$(CXX) -o $@ 15.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
