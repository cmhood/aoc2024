.POSIX:
.SUFFIXES:

default: silver gold

silver: 07.cpp
	$(CXX) -o $@ 07.cpp -DSILVER $(CXXFLAGS)

gold: 07.cpp
	$(CXX) -o $@ 07.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
