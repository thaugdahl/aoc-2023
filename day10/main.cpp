#include <cmath>
#include <format>
#include <iterator>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <ranges>
#include <sstream>
#include <ranges>

#include "Clock.hpp"


#define TEST_TRUE(val) assert((val) == true)



const std::string get_input_filename(int argc, char **argv)
{
    if ( argc < 2 ) {
        return "input.txt";
    } else {
        return std::string(argv[1]);
    }
}

constexpr std::array<char, 6> PIPE_SYMBOLS = {
    '|', '-', 'L', 'J', '7', 'F'
};

enum class Pipe : uint8_t {
    NONE,
    NORTH_TO_SOUTH,
    WEST_TO_EAST,
    NORTH_TO_EAST,
    NORTH_TO_WEST,
    WEST_TO_SOUTH,
    SOUTH_TO_EAST,
    ANIMAL
};

Pipe pipe_from_char(char c)
{
    switch (c) {
        case '|':
            return Pipe::NORTH_TO_SOUTH;
            break;

        case '-':
            return Pipe::WEST_TO_EAST;
            break;

        case 'L':
            return Pipe::NORTH_TO_EAST;
            break;

        case 'J':
            return Pipe::NORTH_TO_WEST;
            break;

        case '7':
            return Pipe::WEST_TO_SOUTH;
            break;

        case 'F':
            return Pipe::SOUTH_TO_EAST;
            break;

        case 'S':
            return Pipe::ANIMAL;
            break;

        default:
            return Pipe::NONE;
            break;
    }
}

using Coordinate = std::pair<int64_t, int64_t>;

struct Chart {
    std::vector<Pipe> data;
    std::size_t width;
    std::size_t height;
    std::vector<bool> visited;

    const Pipe at(const Coordinate coord) const {
        if ( coord.first < 0 || coord.first >= width ) return Pipe::NONE;
        if ( coord.second < 0 || coord.second >= height ) return Pipe::NONE;

        return data[coord.first + coord.second*width];
    };

    void mark_visited(Coordinate coord) {
        if ( coord.first < 0 || coord.first >= width ) return;
        if ( coord.second < 0 || coord.second >= height ) return;

        visited[coord.first + coord.second*width] = true;
    }

    bool is_visited(const Coordinate coord) const {
        if ( coord.first < 0 || coord.first >= width ) return false;
        if ( coord.second < 0 || coord.second >= height ) return false;
        return visited[coord.first + coord.second*width];
    }
};

Chart chart_from_file(std::ifstream &file)
{
        std::string current_line;
        std::vector<Pipe> chart_data;
        std::size_t chart_width = 0;
        while ( std::getline(file, current_line) )
        {
            chart_width = current_line.length();

            for ( auto &c : current_line )
            {
                chart_data.emplace_back(pipe_from_char(c));
            }
        }

        std::size_t chart_height = chart_data.size() / chart_width;

        std::vector<bool> visited(chart_data.size());

        return Chart{std::move(chart_data), chart_width, chart_height, std::move(visited)};
}


Coordinate find_animal(const Chart &chart)
{
    auto pos_iter = std::find(chart.data.begin(), chart.data.end(), Pipe::ANIMAL);

    std::size_t animal_pos_1d = std::distance(chart.data.begin(), pos_iter);

    Coordinate animal_pos = std::make_pair( animal_pos_1d % chart.width, animal_pos_1d / chart.height );

    return animal_pos;
}

bool accepts(Coordinate diff, Pipe landing_pipe)
{
    if ( landing_pipe == Pipe::NONE ) return false;

    if ( diff.first == 1 )
    {
        return landing_pipe == Pipe::WEST_TO_SOUTH
            || landing_pipe == Pipe::WEST_TO_EAST
            || landing_pipe == Pipe::NORTH_TO_WEST;
    }

    if ( diff.first == -1 )
    {
        return  landing_pipe == Pipe::WEST_TO_EAST
            || landing_pipe == Pipe::NORTH_TO_EAST
            || landing_pipe == Pipe::SOUTH_TO_EAST;
    }

    if ( diff.second == 1 )  {
        return landing_pipe == Pipe::NORTH_TO_EAST
            || landing_pipe == Pipe::NORTH_TO_WEST
            || landing_pipe == Pipe::NORTH_TO_SOUTH;
    }

    if ( diff.second == -1 ) {
        return landing_pipe == Pipe::SOUTH_TO_EAST
            || landing_pipe == Pipe::NORTH_TO_SOUTH
            || landing_pipe == Pipe::WEST_TO_SOUTH;
    }

    return false;

}

bool allows(Coordinate diff, Pipe from)
{
    if ( diff.first == 1 )
    {
        return from == Pipe::SOUTH_TO_EAST
            || from == Pipe::NORTH_TO_EAST
            || from == Pipe::WEST_TO_EAST;
    }

    if ( diff.first == -1 )
    {
        return  from == Pipe::WEST_TO_EAST
            || from == Pipe::WEST_TO_SOUTH
            || from == Pipe::NORTH_TO_WEST;
    }

    if ( diff.second == -1 )  {
        return from == Pipe::NORTH_TO_EAST
            || from == Pipe::NORTH_TO_SOUTH
            || from == Pipe::NORTH_TO_WEST;
    }

    if ( diff.second == 1 ) {
        return from == Pipe::SOUTH_TO_EAST
            || from == Pipe::NORTH_TO_SOUTH
            || from == Pipe::WEST_TO_SOUTH;
    }

    return false;
}


std::array<Pipe, 16> LUT = {
    Pipe::NONE,                // 0000
    Pipe::NONE,       // 0001
    Pipe::NONE,      // 0010
    Pipe::WEST_TO_SOUTH,       // 0011
    Pipe::NONE,       // 0100
    Pipe::WEST_TO_EAST,                // 0101
    Pipe::SOUTH_TO_EAST,       // 0110
    Pipe::NONE,        // 0111
    Pipe::NONE,      // 1000
    Pipe::NORTH_TO_WEST,       // 1001
    Pipe::NORTH_TO_SOUTH,                // 1010
    Pipe::NONE,       // 1011
    Pipe::NORTH_TO_EAST,       // 1100
    Pipe::NONE,        // 1101
    Pipe::NONE,       // 1110
    Pipe::NONE,                // 1111
};

Pipe fit_piece(Chart &chart, Coordinate pos)
{
    auto at = [&chart] (int64_t x, int64_t y ) {

        if ( x < 0 || x >= chart.width || y < 0 || y >= chart.height ) return Pipe::NONE;

        return chart.data[y * chart.width + x];
    };

    bool east = accepts({1, 0}, at(pos.first + 1, pos.second));
    bool west = accepts({-1, 0}, at(pos.first - 1, pos.second));
    bool south = accepts({0, 1}, at(pos.first, pos.second + 1));
    bool north = accepts({0, -1}, at(pos.first, pos.second - 1));

    uint8_t mask = north << 3 | east << 2 | south << 1 | west;

    return LUT[mask];
}

std::array<Coordinate, 4> DIRECTION_DIFFS = {
    std::make_pair(0, -1),
    std::make_pair(1, 0),
    std::make_pair(0, 1),
    std::make_pair(-1, 0)
};

std::pair<Coordinate, bool> try_move(const Chart &chart, Coordinate from, Coordinate diff)
{
    Coordinate new_coord = {from.first + diff.first, from.second + diff.second};

    const Pipe pipe_from = chart.at(from);
    const Pipe pipe_to = chart.at(new_coord);

    bool can_move_from = allows(diff, pipe_from);
    bool can_move_to = accepts(diff, pipe_to);

    return std::make_pair(new_coord, can_move_to && can_move_from);
}

Coordinate move_first(const Chart &chart, Coordinate from, Coordinate prev)
{
    for (const auto c : DIRECTION_DIFFS )
    {
        auto [new_coord, valid] = try_move(chart, from, c);

        if (valid && new_coord != prev) return new_coord;
    }

    return from;

}

Coordinate move_second(const Chart &chart, const Coordinate from, const Coordinate prev)
{

    auto iter = DIRECTION_DIFFS.rbegin();
    auto end = DIRECTION_DIFFS.rend();

    while ( iter != end )
    {
        auto [new_coord, valid] = try_move(chart, from, *iter);

        if ( valid && new_coord != prev ) return new_coord;

        iter++;
    }

    return from;

}

bool is_crossing(const Pipe pipe)
{
    return pipe == Pipe::NORTH_TO_WEST
        || pipe == Pipe::NORTH_TO_EAST
        || pipe == Pipe::NORTH_TO_SOUTH;
}

std::size_t solve(Chart &chart, Coordinate starting_pos)
{
    // Do two simultaneous runs, until the two meet, at which point we have the largest distance.


    Coordinate bs_coord = std::make_pair(-1,-1);

    std::array<Coordinate, 2> first_previous{bs_coord, bs_coord};
    std::array<Coordinate, 2> second_previous{bs_coord, bs_coord};

    auto first_prev = starting_pos;
    auto first = move_first(chart, starting_pos, first_previous[1]);

    auto second_prev = starting_pos;
    auto second = move_second(chart, starting_pos, second_previous[1]);

    std::size_t distance = 0;

    chart.mark_visited(starting_pos);


    while ( first != second )
    {
        chart.mark_visited(first);
        chart.mark_visited(second);

        first = move_first(chart, first, first_previous[1]);
        second = move_second(chart, second, second_previous[1]);

        first_previous[1] = first_previous[0];
        second_previous[1] = second_previous[0];

        first_previous[0] = first;
        second_previous[0] = second;

        ++distance;
    }


    return distance;
}


int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);





    if ( ! std::filesystem::exists(inputFile) ) {
        std::cerr << "Input file does not exist\n";
        exit(EXIT_FAILURE);
    };

    std::ifstream file{inputFile};

    if ( ! file.is_open() ) {
        std::cerr << "Failed to open file!" << "\n";
    }


    std::string current_line;

    MicrosecondClock clock;

    clock.start();


    Chart chart = chart_from_file(file);

    Coordinate coord = find_animal(chart);

    auto piece = fit_piece(chart, coord);

    chart.data[coord.first + coord.second * chart.width] = piece;

    auto result = solve(chart, coord);

    auto yrange = std::ranges::views::iota(static_cast<std::size_t>(0), static_cast<std::size_t>(chart.height));
    auto xrange = std::ranges::views::iota(static_cast<std::size_t>(0), static_cast<std::size_t>(chart.width));

    std::size_t contained = 0;
    for ( auto y : yrange )
    {
        std::size_t crossings = 0;
        for ( auto x : xrange )
        {
            if ( is_crossing(chart.at({x,y})) && chart.is_visited({x,y}) ) {
                crossings++;
            }

            // Ray-casting
            if ( crossings % 2 == 1 && ! chart.is_visited({x,y}) ) { contained++; }


        }
    }


    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << std::format("Max distance: {}\n", result);
    std::cout << std::format("Contained: {}\n", contained);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

