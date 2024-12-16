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
#include <iterator>

struct Grid {
	size_t width;
	size_t robot_position;
	std::vector<char> data;
	static Grid parse(std::string_view);
	bool push(size_t, ptrdiff_t, char = '.');
};

std::vector<char> widen(std::vector<char> const &);
static void expect(bool);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t split = input.find("\n\n");
	expect(split != input.npos);

	Grid grid = Grid::parse(input.substr(0, split + 1));

	std::vector<char> robot_directions;
	std::string_view sv = input.substr(split + 2);
	std::copy_if(sv.begin(), sv.end(), std::back_inserter(robot_directions),
	    [](char c) -> bool {
		return c && strchr("<>^v", c);
	});

	auto get_dir = [&](char c) -> ptrdiff_t {
		switch (c) {
		case '^':
			return -grid.width;
		case 'v':
			return grid.width;
		case '<':
			return -1;
		case '>':
			return 1;
		default:
			assert(false);
		}
	};

	for (char c : robot_directions) {
		ptrdiff_t dir = get_dir(c);
		std::vector<char> old_state = grid.data;
		if (grid.push(grid.robot_position, dir)) {
			grid.robot_position += dir;
		} else {
			grid.data = old_state;
		}
	}

	int64_t sum = 0;
	for (size_t i = 0; i < grid.data.size(); ++i) {
		if (grid.data[i] == 'O' || grid.data[i] == '[') {
			sum += (i % grid.width) + (i / grid.width) * 100;
		}
	}
	printf("%" PRIi64 "\n", sum);
}

bool
Grid::push(size_t pos, ptrdiff_t dir, char c)
{
	char here = data[pos];
	int p = here == '[' ? 1 : (here == ']' ? -1 : 0);
	switch (here) {
	case '#':
		return false;
	case 'O':
	case '@':
	case '[':
	case ']':
		data[pos + p] = '.';
		data[pos] = c;
		return push(pos + dir, dir, here) && (!p ||
		    push(pos + dir + p, dir, "[ ]"[p + 1]));
	case '.':
		data[pos] = c;
		return true;
	default:
		assert(false);
	}
}

Grid
Grid::parse(std::string_view sv)
{
	Grid grid;

	grid.width = sv.find('\n');
	expect(grid.width != sv.npos);

	std::copy_if(sv.begin(), sv.end(), std::back_inserter(grid.data),
	    [](char c) -> bool {
		return c && strchr("#@O.", c);
	});
	expect(grid.data.size() % grid.width == 0);

#ifdef GOLD
	grid.width *= 2;
	grid.data = widen(grid.data);
#endif

	grid.robot_position = SIZE_MAX;
	for (size_t i = 0; i < grid.data.size(); ++i) {
		char c = grid.data[i];
		if (i % grid.width == 0 || i % grid.width == grid.width - 1 ||
		    i < grid.width || i + grid.width >= grid.data.size()) {
			expect(c == '#');
		}
		if (grid.data[i] == '@') {
			expect(grid.robot_position == SIZE_MAX);
			grid.robot_position = i;
		}
	}

	return grid;
}

std::vector<char>
widen(std::vector<char> const &grid)
{
	std::vector<char> res;
	for (char c : grid) {
		char const *str;
		switch (c) {
		case '#':
			str = "##";
			break;
		case 'O':
			str = "[]";
			break;
		case '.':
			str = "..";
			break;
		case '@':
			str = "@.";
			break;
		default:
			assert(false);
		}
		res.push_back(str[0]);
		res.push_back(str[1]);
	}
	return res;
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
