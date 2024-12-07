.POSIX:
.SUFFIXES:

default: silver gold

silver: 08.cpp
	$(CXX) -o $@ 08.cpp -DSILVER $(CXXFLAGS)

gold: 08.cpp
	$(CXX) -o $@ 08.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
