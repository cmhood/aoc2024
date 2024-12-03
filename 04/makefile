.POSIX:
.SUFFIXES:

default: silver gold

silver: 04.cpp
	$(CXX) -o $@ 04.cpp -DSILVER $(CXXFLAGS)

gold: 04.cpp
	$(CXX) -o $@ 04.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
