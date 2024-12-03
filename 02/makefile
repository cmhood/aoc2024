.POSIX:
.SUFFIXES:

default: silver gold

silver: 02.cpp
	$(CXX) -o $@ 02.cpp -DSILVER $(CXXFLAGS)

gold: 02.cpp
	$(CXX) -o $@ 02.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
