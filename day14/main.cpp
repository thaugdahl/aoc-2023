#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <unordered_map>

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

static constexpr std::size_t NUM_CYCLES = 1e9;

// Instead of a naive upper bound iteration scheme
// all rocks could be moved at once by identifying how many rocks
// are between one rock and the next and placing them directly adjacent
// to the rock.
// I did it quick and dirty.

void tilt_north(std::vector<std::string> &map)
{
    std::size_t map_width = map[0].length();

    for ( std::size_t iter = 0; iter < map.size(); iter++ )
    {
        auto trailing = map.begin();
        auto leading = trailing + 1;
        auto end = map.end();

        while ( leading != end )
        {
            for ( std::size_t i = 0; i < map_width; i++)
            {
                // Can move north?
                char &northern = (*trailing)[i];
                char &southern = (*leading)[i];

                if ( northern == '#' ) {
                    // Can't move stuff further north
                }

                if ( northern == '.' && southern != '#' ) {
                    std::swap(northern, southern);
                }
            }

            leading++;
            trailing++;
        }
    }
}


void tilt_south(std::vector<std::string> &map)
{
    std::size_t map_width = map[0].length();

    for ( std::size_t iter = 0; iter < map.size(); iter++ )
    {
        auto trailing = map.rbegin();
        auto leading = trailing + 1;
        auto end = map.rend();

        while ( leading != end )
        {
            for ( std::size_t i = 0; i < map_width; i++)
            {
                // Can move north?
                char &northern = (*leading)[i];
                char &southern = (*trailing)[i];

                if ( southern == '#' ) {
                    // Can't move stuff further north
                }

                if ( southern == '.' && northern != '#' ) {
                    std::swap(northern, southern);
                }
            }

            leading++;
            trailing++;
        }
    }
}
void tilt_east(std::vector<std::string> &map)
{
    std::size_t map_width = map[0].length();
    std::size_t map_height = map.size();

    for ( std::size_t iter = 0; iter < map_width; iter++)
    for ( std::size_t y = 0; y < map_height; y++)
    {
        for ( std::size_t x = 1; x < map_width; x++)
        {
            char &west = map[y][x-1];
            char &east = map[y][x];

            if ( east == '.' && west != '#' )
            {
                std::swap(east, west);
            }
        }
    }
}

void tilt_west(std::vector<std::string> &map)
{
    std::size_t map_width = map[0].length();
    std::size_t map_height = map.size();

    for ( std::size_t iter = 0; iter < map_width; iter++)
    for ( std::size_t y = 0; y < map_height; y++)
    {
        for ( std::size_t x = map_width-1; x > 0; x--)
        {
            char &west = map[y][x-1];
            char &east = map[y][x];

            if ( west == '.' && east != '#' )
            {
                std::swap(east, west);
            }
        }
    }
}

std::size_t calculate_load(const std::vector<std::string> &map)
{
    std::size_t height = map.size();
    std::size_t width = map[0].length();
    std::size_t result = 0;

    for ( std::size_t i = 0; i < height; i++)
    {
        std::size_t row_load_factor = height - i;
        for ( std::size_t j = 0; j < width; j++)
        {
            if ( map[i][j] == 'O' ) {
                result += row_load_factor;
            }
        }
    }

    return result;
}


void print_map(const std::vector<std::string> &map)
{
    for ( auto &s : map )
    {
        std::cout << s << "\n";
    }
}

std::string get_hash(const std::vector<std::string> &map)
{
    std::string res = "";
    for ( auto &s : map )
    {
        res += s;
    }

    return res;
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

    std::vector<std::string> rock_map{};

    std::string current_line;

    MicrosecondClock clock;

    clock.start();


    while ( std::getline(file, current_line ) )
    {
        rock_map.emplace_back(std::move(current_line));
    }

    std::size_t map_width = rock_map[0].length();

    auto original_map = rock_map;

    std::unordered_map<std::string, std::size_t> seen{};
    std::vector<std::vector<std::string>> grid_at_cycle{};


    std::vector<std::string> result_grid{};

    for ( std::size_t cycle = 0; cycle < NUM_CYCLES; cycle++ )
    {
        if ( cycle % 100 == 0 ) std::cout << cycle << "\n";

        tilt_north(rock_map);
        tilt_west(rock_map);
        tilt_south(rock_map);
        tilt_east(rock_map);

        auto hash = get_hash(rock_map);

        if ( seen.contains(hash) )
        {
            auto seen_cycle = seen.at(hash);
            std::size_t period = cycle - seen_cycle;

            // (NUM_CYCLES - 1 - seen_cycle) % period -- 0-index offset into the period
            // + seen_cycle -- Offset to the beginning of the first cycle.
            std::size_t solution_index = (NUM_CYCLES - 1 - seen_cycle) % period + seen_cycle;

            result_grid = grid_at_cycle[solution_index];

            break;
        }

        seen.emplace(std::make_pair(hash, cycle));
        grid_at_cycle.push_back(rock_map);
    }


    auto result = calculate_load(result_grid);


    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << std::format("Result: {}\n", result);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

