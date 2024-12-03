.POSIX:
.SUFFIXES:

default: silver gold

silver: 03.cpp
	$(CXX) -o $@ 03.cpp -DSILVER $(CXXFLAGS)

gold: 03.cpp
	$(CXX) -o $@ 03.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
