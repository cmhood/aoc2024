.POSIX:
.SUFFIXES:

default: silver gold

silver: 12.cpp
	$(CXX) -o $@ 12.cpp -DSILVER $(CXXFLAGS)

gold: 12.cpp
	$(CXX) -o $@ 12.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
