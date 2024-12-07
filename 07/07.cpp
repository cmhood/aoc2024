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
	double lhs;
	std::vector<double> rhs;
};

static bool has_permutation(double, double, size_t, double const *);
static double concatenate(double, double);
static std::string_view get_input(int, char **);
static std::optional<Equation> parse_equation(std::string_view, size_t *);
static double parse_int(std::string_view, size_t *);
static bool parse_char(std::string_view, size_t *, char);
static void expect(bool);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t offset = 0;

	double count = 0;
	while (std::optional<Equation> eq = parse_equation(input, &offset)) {
		std::vector<double> const &v = eq->rhs;
		if (has_permutation(eq->lhs, v[0], v.size() - 1, &v[1])) {
			count += eq->lhs;
		}
	}
	printf("%" PRIi64 "\n", (int64_t)count);
}

static bool
has_permutation(double target, double partial, size_t count, double const *a)
{
	if (partial > target || count == 0) {
		return partial == target && count == 0;
	}
	double n = a[0];
#ifdef SILVER
	std::array<double, 2> options = {partial + n, partial * n};
#else
	std::array<double, 3> options =
	    {partial + n, partial * n, concatenate(partial, n)};
#endif
	for (double opt : options) {
		if (has_permutation(target, opt, count - 1, &a[1])) {
			return true;
		}
	}
	return false;
}

static double
concatenate(double l, double r)
{
	double res = (double)pow(10.0, ceil(log10(r + 1))) * l + r;
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
		double n = parse_int(input, offset);
		eq.rhs.push_back(n);
		if (parse_char(input, offset, '\n')) {
			return eq;
		}
		expect(parse_char(input, offset, ' '));
	}
}

static double
parse_int(std::string_view input, size_t *offset)
{
	double i;
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
