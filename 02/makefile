.POSIX:
.SUFFIXES:

.PHONY: default
default: silver gold

silver: 02.cpp
	$(CXX) -o $@ 02.cpp -DSILVER $(CXXFLAGS)

gold: 02.cpp
	$(CXX) -o $@ 02.cpp -DGOLD $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f silver gold
