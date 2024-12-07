#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <array>
#include <vector>
#include <optional>
#include <charconv>

struct Equation {
	int64_t lhs;
	std::vector<int64_t> rhs;
};

static bool has_permutation(int64_t, int64_t, size_t, int64_t const *);
static int64_t concatenate(int64_t, int64_t);
static std::string_view get_input(int, char **);
static std::optional<Equation> parse_equation(std::string_view, size_t *);
static int64_t parse_int(std::string_view, size_t *);
static bool parse_char(std::string_view, size_t *, char);
static void expect(bool);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t offset = 0;

	int64_t count = 0;
	while (std::optional<Equation> eq = parse_equation(input, &offset)) {
		std::vector<int64_t> const &v = eq->rhs;
		if (has_permutation(eq->lhs, v[0], v.size() - 1, &v[1])) {
			count += eq->lhs;
		}
	}
	printf("%" PRIi64 "\n", count);
}

static bool
has_permutation(int64_t target, int64_t partial, size_t count, int64_t const *a)
{
	if (partial > target || count == 0) {
		return partial == target && count == 0;
	}
	int64_t n = a[0];
#ifdef SILVER
	std::array<int64_t, 2> options = {partial + n, partial * n};
#else
	std::array<int64_t, 3> options =
	    {partial + n, partial * n, concatenate(partial, n)};
#endif
	for (int64_t opt : options) {
		if (has_permutation(target, opt, count - 1, &a[1])) {
			return true;
		}
	}
	return false;
}

static int64_t
concatenate(int64_t l, int64_t r)
{
	int64_t res = (int64_t)pow(10.0, ceil(log10(r + 1))) * l + r;
	return res;
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

static std::optional<Equation>
parse_equation(std::string_view input, size_t *offset)
{
	if (*offset >= input.size()) {
		return {};
	}

	Equation eq;

	eq.lhs = parse_int(input, offset);
	expect(parse_char(input, offset, ':'));
	expect(parse_char(input, offset, ' '));

	for (;;) {
		int64_t n = parse_int(input, offset);
		eq.rhs.push_back(n);
		if (parse_char(input, offset, '\n')) {
			return eq;
		}
		expect(parse_char(input, offset, ' '));
	}
}

static int64_t
parse_int(std::string_view input, size_t *offset)
{
	int64_t i;
	std::from_chars_result r =
	    std::from_chars(&input[*offset], &input[input.size()], i);
	expect(r.ec == std::errc());
	*offset = r.ptr - input.data();
	return i;
}

static bool
parse_char(std::string_view input, size_t *offset, char c)
{
	expect(*offset < input.size());
	bool b = input[*offset] == c;
	*offset += b;
	return b;
}

static void
expect(bool cond)
{
	if (!cond) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
}
