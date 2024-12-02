.POSIX:
.SUFFIXES:

.PHONY: default
default: silver gold

silver: 01.c
	$(CC) -o $@ 01.c -DSILVER $(CFLAGS)

gold: 01.c
	$(CC) -o $@ 01.c -DGOLD $(CFLAGS)

.PHONY: clean
clean:
	rm -f silver gold
