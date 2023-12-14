// DISCLAIMER:
// Day 12 heavily loans from
// [locked out](https://github.com/locked-out/adventofcode23/blob/main/12/2.cpp)
//
// I needed some help wrapping my head around how I should memoize.
// Thank you for open-sourcing your code.
#include <format>
#include <numeric>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <sstream>

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

struct LineConfig {
    std::string states;
    std::vector<int> groups;
};

LineConfig read_line_config(const std::string &str)
{
        std::string configuration = str.substr(0, str.find(' '));
        std::string contiguous_lengths_str = str.substr(str.find(' ') + 1);

        std::istringstream iss{contiguous_lengths_str};
        std::string token;

        std::vector<int> contiguous_lengths{};
        while ( std::getline(iss, token, ',') )
        {
            try {
                contiguous_lengths.emplace_back(std::stoi(token));
            } catch(const std::exception &e) {}
        }


        return LineConfig{std::move(configuration), std::move(contiguous_lengths)};
}

LineConfig unfold_line(const LineConfig &lc)
{
    std::string unfolded_states{};
    std::vector<int> unfolded_groups{};


    for ( std::size_t i = 0; i < 4; i++ )
    {
        unfolded_states += lc.states + "?";

        for ( auto g : lc.groups )
        {
            unfolded_groups.emplace_back(g);
        }

    }

    unfolded_states += lc.states;

    for ( auto g : lc.groups )
    {
        unfolded_groups.emplace_back(g);
    }

    return LineConfig{std::move(unfolded_states), std::move(unfolded_groups)};
}

constexpr std::size_t DOT = 0;
constexpr std::size_t HASH = 1;

std::size_t solve_line(const LineConfig &config)
{

    auto &springs = config.states;
    auto groups = config.groups;

    auto n = springs.length();
    auto num_groups = groups.size();

    // Offset each group by one
    std::vector<bool> is_group_ending(n+1);
    is_group_ending[0] = true;

    std::size_t group_acc = 0;
    for ( auto g : groups )
    {
        group_acc += g;
        is_group_ending[group_acc] = true;
    }

    auto memo_matrix = new std::size_t[n+1][100][100]{0};

    std::size_t num_hashes = std::accumulate(groups.begin(), groups.end(), 0);

    // Leading implicit dot
    // Dimensions: [INDEX][Hashes Placed Before][Symbol at index]
    memo_matrix[0][0][DOT] = 1;

    for ( std::size_t i = 1; i <= n; i++ )
    {
        // If it's not a dot, it is either a ? or #
        // Explore ways of placing a #
        if ( springs[i-1] != '.' )
        {
            for ( int hashes_placed = 1; hashes_placed <= std::min(i, num_hashes); hashes_placed++ )
            {
                // Check group ending.
                if ( is_group_ending[hashes_placed - 1] ) {
                    // This placed hash starts a new group. The preceding character must have been a dot.
                    memo_matrix[i][hashes_placed][HASH] = memo_matrix[i-1][hashes_placed-1][DOT];
                } else {
                    // We are currently in a group. The preceding must be a hash.
                    memo_matrix[i][hashes_placed][HASH] = memo_matrix[i-1][hashes_placed-1][HASH];
                }
            }
        }


        if ( springs[i-1] != '#' )
        {
            for ( int hashes_placed = 0; hashes_placed <= std::min(i-1, num_hashes); hashes_placed++ )
            {
                // The number of hashes placed till now is at the end of a group. The next symbol is a dot.
                if ( is_group_ending[hashes_placed] ) {
                    memo_matrix[i][hashes_placed][DOT] = memo_matrix[i-1][hashes_placed][DOT] + memo_matrix[i-1][hashes_placed][HASH];
                } else {
                    // We are not in between two groups. Can't place a dot.
                }
            }

        }
    }

    auto result = memo_matrix[n][num_hashes][DOT] + memo_matrix[n][num_hashes][HASH];

    delete[] memo_matrix;
    return result;

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

    std::vector<LineConfig> line_configs;

    while ( std::getline(file, current_line ) )
    {
        line_configs.emplace_back(unfold_line(read_line_config(current_line)));
    }

    std::size_t sum = 0;
    for ( auto &l : line_configs )
    {
        std::size_t arrangements = solve_line(l);

        sum += arrangements;
    }




    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << "Solution: " << sum << "\n";
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

