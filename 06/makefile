.POSIX:
.SUFFIXES:

default: silver gold

silver: 06.cpp
	$(CXX) -o $@ 06.cpp -DSILVER $(CXXFLAGS)

gold: 06.cpp
	$(CXX) -o $@ 06.cpp -DGOLD $(CXXFLAGS)

clean:
	rm -f silver gold

.PHONY: default clean
