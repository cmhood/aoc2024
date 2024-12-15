.POSIX:
.SUFFIXES:

default: silver gold

silver: 14.cpp
	$(CXX) -o $@ 14.cpp -DSILVER $(CXXFLAGS)

gold: 14.cpp
	$(CXX) -o $@ 14.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
