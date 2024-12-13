.POSIX:
.SUFFIXES:

default: silver gold

silver: 13.cpp
	$(CXX) -o $@ 13.cpp -DSILVER $(CXXFLAGS)

gold: 13.cpp
	$(CXX) -o $@ 13.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
