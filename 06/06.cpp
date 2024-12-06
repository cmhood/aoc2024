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
#include <set>
#include <algorithm>

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
	int width = newline - input.data() + 1;

	char const *ptr = static_cast<char const *>(memchr(
	    static_cast<void const *>(input.data()), '^', input.size()));
	size_t guard_position = ptr - input.data();
	ptrdiff_t guard_direction = -width;

	std::set<size_t> traversed_positions;
	traversed_positions.insert(guard_position);
	while (!(guard_direction > static_cast<ptrdiff_t>(guard_position) ||
	    guard_direction + guard_position >= input.size() ||
	    input[guard_direction + guard_position] == '\n')) {
/*
		for (size_t i = 0; i < width - 1; ++i) {
			putchar('=');
		}
		putchar('\n');
		for (size_t i = 0; i < input.size(); ++i) {
			char c = input[i];
			if (c == '^') {
				c = '.';
			}
			if (i == guard_position) {
				c = '@';
			}
			putchar(c);
		}
*/

		if (input[guard_position + guard_direction] == '#') {
			int sgn = guard_direction >= 0 ? 1 : -1;
			int mag = guard_direction * sgn;
			guard_direction = sgn * (mag == 1 ? width : -1);
		} else {
			guard_position += guard_direction;
			traversed_positions.insert(guard_position);
		}
	}
	printf("%zu\n", traversed_positions.size());
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
