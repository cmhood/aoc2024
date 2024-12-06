#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <array>
#include <map>
#include <set>

static std::map<size_t, int>
    traverse(std::string_view, size_t, size_t, size_t = SIZE_MAX);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

	char const *newline = static_cast<char const *>(memchr(
	    static_cast<void const *>(input.data()), '\n', input.size()));
	if (!newline) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
	size_t width = newline - input.data() + 1;

	char const *ptr = static_cast<char const *>(memchr(
	    static_cast<void const *>(input.data()), '^', input.size()));
	size_t guard_pos = ptr - input.data();

	std::map<size_t, int> traversal = traverse(input, width, guard_pos);

#ifdef SILVER
	printf("%zu\n", traversal.size());
#else
	std::set<size_t> obstacles;
	for (auto const &i : traversal) {
		size_t obs = i.first;
		if (obs == guard_pos) {
			continue;
		}
		if (traverse(input, width, guard_pos, obs).empty()) {
			obstacles.insert(obs);
		}
	}
	printf("%zu\n", obstacles.size());
#endif
}

static std::map<size_t, int>
traverse(std::string_view map, size_t w, size_t pos, size_t obs)
{
	int dir = 0;
	std::map<size_t, int> traversal;
	for (;;) {
		ptrdiff_t wi = static_cast<ptrdiff_t>(w);
		std::array<ptrdiff_t, 4> a = {-wi, 1, wi, -1};
		ptrdiff_t d = a[dir];

		if (d > static_cast<ptrdiff_t>(pos) || d + pos >= map.size() ||
		    map[pos + d] == '\n') {
			++traversal[pos];
			break;
		}
		if (pos + d == obs || map[pos + d] == '#') {
			dir = (dir + 1) % 4;
			continue;
		}
		++traversal[pos];
		if (traversal[pos] > 4) {
			return {};
		}
		pos += d;
	}
	return traversal;
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
