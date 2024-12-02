.POSIX:
.SUFFIXES:

.PHONY: default
default: silver gold

silver: 02.c
	$(CC) -o $@ 02.c -DSILVER $(CFLAGS)

gold: 02.c
	$(CC) -o $@ 02.c -DGOLD $(CFLAGS)

.PHONY: clean
clean:
	rm -f silver gold
