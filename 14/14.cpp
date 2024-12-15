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
#include <charconv>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>

constexpr int64_t grid_width = 101;
constexpr int64_t grid_height = 103;
constexpr int64_t grid_size = grid_width * grid_height;

struct Coords {
	int64_t x;
	int64_t y;
	Coords operator+(Coords const &) const;
	Coords operator*(int64_t const &) const;
	Coords canonical() const;
};

struct Robot {
	Coords position;
	Coords velocity;
	static Robot parse(std::string_view, size_t *);
};

static bool has_contiguous(std::array<bool, grid_width> const &);
static int64_t parse_int(std::string_view, size_t *);
static void parse_string(std::string_view, size_t *, char const *);
static void expect(bool);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t offset = 0;

	std::vector<Robot> robots;
	while (offset < input.size()) {
		robots.push_back(Robot::parse(input, &offset));
	}

#ifdef SILVER
	std::array<int64_t, 4> quads = {};
	for (Robot const &robot : robots) {
		Coords p = (robot.position + robot.velocity * 100).canonical();
		if (p.x == grid_width / 2 || p.y == grid_height / 2) {
			continue;
		}
		++quads[(p.x < grid_width / 2) | 2 * (p.y < grid_height / 2)];
	}
	int64_t safety_factor = std::accumulate(quads.begin(), quads.end(),
	    1, std::multiplies<int64_t>());
	printf("%" PRIi64 "\n", safety_factor);
#else
	int64_t time = 0;
	for (;; ++time) {
		std::array<std::array<bool, grid_width>, grid_height> grid = {};
		for (Robot const &robot : robots) {
			Coords p = (robot.position + robot.velocity * time)
			    .canonical();
			grid[p.y][p.x] = true;
		}
		if (std::any_of(grid.begin(), grid.end(), has_contiguous)) {
			break;
		}
	}
	printf("%" PRIi64 "\n", time);
#endif
}

static bool
has_contiguous(std::array<bool, grid_width> const &row)
{
	for (size_t i = 0; i < row.size(); ++i) {
		if (!row[i]) {
			continue;
		}
		size_t j = i;
		while (row[j]) {
			++j;
		}
		if (j - i > 16) {
			return true;
		}
	}
	return false;
}

Robot
Robot::parse(std::string_view input, size_t *offset)
{
	constexpr std::array<char const *, 4> strings = {"p=", ",", " v=", ","};
	std::array<int64_t, strings.size()> ints;
	for (size_t i = 0 ; i < strings.size(); ++i) {
		parse_string(input, offset, strings[i]);
		ints[i] = parse_int(input, offset);
	}
	parse_string(input, offset, "\n");
	return {
		{ints[0], ints[1]},
		{ints[2] + grid_width, ints[3] + grid_height}
	};
}

Coords
Coords::operator+(Coords const &other) const
{
	return {this->x + other.x, this->y + other.y};
}

Coords
Coords::operator*(int64_t const &k) const
{
	return {this->x * k, this->y * k};
}

Coords
Coords::canonical() const
{
	return {this->x % grid_width, this->y % grid_height};
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
