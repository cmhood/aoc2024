.POSIX:
.SUFFIXES:

default: silver gold

silver: 09.cpp
	$(CXX) -o $@ 09.cpp -DSILVER $(CXXFLAGS)

gold: 09.cpp
	$(CXX) -o $@ 09.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
