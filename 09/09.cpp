#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <vector>

using Block = int32_t;
using Filesystem = std::vector<Block>;

static Filesystem get_filesystem(std::string_view);
static void compact(Filesystem *);
static void insert(Filesystem *, Block);
static bool insert_file(Filesystem *, Block, size_t, size_t);
static int64_t get_checksum(Filesystem const &);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	Filesystem fs = get_filesystem(input);
	compact(&fs);
	printf("%" PRIi64 "\n", get_checksum(fs));
}

static Filesystem
get_filesystem(std::string_view map)
{
	Filesystem fs;
	for (size_t i = 0; i < map.size() - 1; ++i) {
		char c = map[i];
		if (!('0' <= c && c <= '9')) {
			fprintf(stderr, "invalid input\n");
			exit(EXIT_FAILURE);
		}
		int n = c - '0';
		for (int j = 0; j < n; ++j) {
			fs.push_back(i % 2 == 0 ? i / 2 : -1);
		}
	}
	return fs;
}

static void
compact(Filesystem *fs)
{
#ifdef SILVER
	for (size_t i = fs->size() - 1; i > 0; --i) {
		if ((*fs)[i] == -1) {
			continue;
		}
		Block b = (*fs)[i];
		(*fs)[i] = -1;
		insert(fs, b);
	}
#else
	for (size_t i = fs->size(); i > 0;) {
		if ((*fs)[i - 1] == -1) {
			--i;
			continue;
		}

		size_t j = i;
		while (j > 0 && (*fs)[j - 1] == (*fs)[i - 1]) {
			--j;
		}

		Block b = (*fs)[j];
		size_t length = i - j;
		i = j;
		if (!insert_file(fs, b, length, j)) {
			continue;
		}
		for (size_t k = 0; k < length; ++k) {
			(*fs)[j + k] = -1;
		}
	}
#endif
}

static void
insert(Filesystem *fs, Block block)
{
	size_t i = 0;
	while ((*fs)[i] != -1) {
		++i;
	}
	(*fs)[i] = block;
}

static bool
insert_file(Filesystem *fs, Block block, size_t length, size_t limit)
{
	size_t i = 0;
	for (size_t j = 0; j < length;) {
		if (i >= limit) {
			return false;
		}
		if ((*fs)[i + j] == -1) {
			++j;
			continue;
		}
		++i;
		j = 0;
	}
	for (size_t j = 0; j < length; ++j) {
		(*fs)[i + j] = block;
	}
	return true;
}

static int64_t
get_checksum(Filesystem const &fs)
{
	int64_t sum = 0;
	for (size_t i = 0; i < fs.size(); ++i) {
		if (fs[i] == -1) {
			continue;
		}
		sum += i * fs[i];
	}
	return sum;
}

static std::string_view
get_input(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "expected 1 argument\n");
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	off_t length = lseek(fd, 0, SEEK_END);
	if (length < 0) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	void *data = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	return {static_cast<char const *>(data), static_cast<size_t>(length)};
}
