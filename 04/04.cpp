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
#include <array>
#include <algorithm>

static std::string_view get_input(int, char **);
static size_t find_matches(std::string_view, size_t);
static bool is_match(std::string_view, size_t, bool);
static bool is_match(std::array<char, 5> const &);

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

	size_t word_count = 0;
	for (size_t i = 0; i < input.size(); ++i) {
		word_count += find_matches(input.substr(i), width);
	}
	printf("%zu\n", word_count);
}

static size_t
find_matches(std::string_view s, size_t width)
{
#ifdef SILVER
	std::array<size_t, 4> const spacings = {1, width - 1, width, width + 1};
	size_t word_count = 0;
	for (size_t sp : spacings) {
		if (s.size() <= 3 * sp) {
			break;
		}
		word_count += is_match(s, sp, true) + is_match(s, sp, false);
	}
	return word_count;
#else
	std::array<size_t, 5> const pattern =
	    {0, 2, width + 1, 2 * width, 2 * width + 2};
	if (s.size() <= pattern[4]) {
		return 0;
	}
	std::array<char, 5> text;
	std::transform(pattern.begin(), pattern.end(), text.begin(),
	     [&](size_t n) { return s[n]; });
	return is_match(text);
#endif
}

static bool
is_match(std::string_view s, size_t spacing, bool reverse)
{
	std::string_view const text = "XMAS";
	for (size_t i = 0; i < text.size(); ++i) {
		if (s[spacing * i] != text[reverse ? text.size() - i - 1 : i]) {
			return false;
		}
	}
	return true;
}

static bool
is_match(std::array<char, 5> const &text)
{
	for (size_t i = 0; i < text.size(); ++i) {
		char c = text[i];
		if (i == 2 ? c != 'A' : (c != 'M' && c != 'S')) {
			return false;
		}
	}
	return text[0] != text[4] && text[1] != text[3];
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
