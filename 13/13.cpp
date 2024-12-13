#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <vector>
#include <charconv>

struct Coords {
	int64_t x;
	int64_t y;
	Coords operator+(Coords const &);
	Coords operator-(Coords const &);
	Coords operator*(int64_t const &);
	bool operator<=(Coords const &);
	bool operator==(Coords const &);
};

struct Machine {
	Coords a;
	Coords b;
	Coords p;
	int64_t solve();
	static Machine parse(std::string_view, size_t *);
};

static int64_t parse_int(std::string_view, size_t *);
static void parse_string(std::string_view, size_t *, char const *);
static void expect(bool);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

	std::vector<Machine> machines;
	size_t offset = 0;
	for (;;) {
		machines.push_back(Machine::parse(input, &offset));
		if (offset == input.size()) {
			break;
		}
		parse_string(input, &offset, "\n");
	}

	int64_t sum = 0;
	for (Machine machine : machines) {
		sum += machine.solve();
	}
	printf("%" PRIi64 "\n", sum);
}

int64_t
Machine::solve()
{
#ifdef SILVER
	double offset = 0;
#else
	double offset = 10000000000000;
#endif

	double x = a.x;
	double y = b.x;
	double z = p.x + offset;
	double u = a.y;
	double v = b.y;
	double w = p.y + offset;

	double na = (z - y * w / v) / (x - y * u / v);
	double nb = (z - x * w / u) / (y - x * v / u);
	double ra = round(na);
	double rb = round(nb);
	double eps = pow(2, -10);
	if ((ra - eps < na && na < ra + eps) &&
	    (rb - eps < nb && nb < rb + eps)) {
		return 3 * ra + rb;
	}
	return 0;
}

Machine
Machine::parse(std::string_view input, size_t *offset)
{
	Machine m;
	parse_string(input, offset, "Button A: X+");
	m.a.x = parse_int(input, offset);
	parse_string(input, offset, ", Y+");
	m.a.y = parse_int(input, offset);
	parse_string(input, offset, "\nButton B: X+");
	m.b.x = parse_int(input, offset);
	parse_string(input, offset, ", Y+");
	m.b.y = parse_int(input, offset);
	parse_string(input, offset, "\nPrize: X=");
	m.p.x = parse_int(input, offset);
	parse_string(input, offset, ", Y=");
	m.p.y = parse_int(input, offset);
	parse_string(input, offset, "\n");
	return m;
}

Coords
Coords::operator+(Coords const &other)
{
	return {this->x + other.x, this->y + other.y};
}

Coords
Coords::operator-(Coords const &other)
{
	return {this->x - other.x, this->y - other.y};
}

Coords
Coords::operator*(int64_t const &other)
{
	return {this->x * other, this->y * other};
}

bool
Coords::operator<=(Coords const &other)
{
	return this->x <= other.x && this->y <= other.y;
}

bool
Coords::operator==(Coords const &other)
{
	return this->x == other.x && this->y == other.y;
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

static void
parse_string(std::string_view input, size_t *offset, char const *str)
{
	size_t len = strlen(str);
	expect(*offset + len <= input.size());
	for (size_t i = 0; i < len; ++i) {
		expect(input[*offset + i] == str[i]);
	}
	*offset += len;
}

static void
expect(bool cond)
{
	if (!cond) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
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
