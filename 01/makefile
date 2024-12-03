.POSIX:
.SUFFIXES:

.PHONY: default
default: silver gold

silver: 01.cpp
	$(CXX) -o $@ 01.cpp -DSILVER $(CXXFLAGS)

gold: 01.cpp
	$(CXX) -o $@ 01.cpp -DGOLD $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f silver gold
