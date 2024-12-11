.POSIX:
.SUFFIXES:

default: silver gold

silver: 11.cpp
	$(CXX) -o $@ 11.cpp -DSILVER $(CXXFLAGS)

gold: 11.cpp
	$(CXX) -o $@ 11.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
