#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

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

struct Pattern {
    std::size_t width;
    std::size_t height;

    std::vector<std::string> pattern;
};

std::size_t string_metric(std::string a, std::string b)
{
    std::size_t result = 0;

    result += std::abs(static_cast<int>(a.length() - b.length()));

    std::size_t common_length = std::min(a.length(), b.length());

    for ( std::size_t i = 0; i < common_length; i++)
    {
        result += static_cast<int>(a[i] != b[i]);
    }

    return result;
}

std::size_t column_distance(const Pattern &pattern, std::size_t col1, std::size_t col2)
{
    std::size_t result = 0;

    for ( auto &s : pattern.pattern )
    {
        result += static_cast<int>(s[col1] != s[col2]);
    }

    return result;
}


bool validate_row_reflection(const Pattern &pattern, std::size_t row, int64_t max_dist=0)
{
    if ( row  >= pattern.height ) return false;

    int i = row - 1;
    int j = row + 2;

    if ( i < 0 || j >= pattern.height ) return true;

    int dist_acc = 0;

    while ( i >= 0 && j < pattern.height )
    {
        int dist = string_metric(pattern.pattern[i], pattern.pattern[j]);
        dist_acc += dist;

        if ( dist_acc > max_dist ) return false;

        i--; j++;
    }

    return true;
}

bool validate_col_reflection(const Pattern &pattern, std::size_t col, int64_t max_dist = 0)
{

    int left = col;
    int right = col+1;

    std::size_t dist_acc = 0;

    while ( left >= 0 && right < pattern.width )
    {
        dist_acc += column_distance(pattern, left, right);

        if ( dist_acc > max_dist ) return false;

        left--; right++;
    }

    return true;
}

int64_t find_row_reflection(const Pattern &pattern, int64_t ignore=-1, int64_t max_dist=0)
{
    auto begin = pattern.pattern.begin();
    auto trailing = pattern.pattern.begin();
    auto leading = pattern.pattern.begin() + 1;
    auto end = pattern.pattern.end();

    while ( leading != end )
    {
        std::size_t dist = string_metric(*trailing, *leading);
        if (dist > max_dist)
        {
            trailing++;
            leading++;
            continue;
        }

        std::size_t row = std::distance(begin, trailing);

        trailing++;
        leading++;

        if ( row == ignore ) continue;

        if ( validate_row_reflection(pattern, row, max_dist) ) {
            return row;
        }

    }


    return -1;
}

int64_t find_col_reflection(const Pattern &pattern, int64_t ignore=-1, int64_t max_dist = 0)
{
    for ( std::size_t i = 1; i < (*pattern.pattern.begin()).length(); i++)
    {

        if ( i-1 == ignore ) continue;

        std::size_t dist = column_distance(pattern, i-1, i);

        if ( column_distance(pattern, i-1, i) <= max_dist )
        {
            if ( validate_col_reflection(pattern, i-1, max_dist) ) return i-1;
        }

    }

    return -1;
}


std::size_t solve_pattern(const Pattern &pattern)
{
    std::size_t result = 0;

    int64_t row = find_row_reflection(pattern);
    int64_t col = find_col_reflection(pattern);

    result += (row+1) * 100;
    result += (col+1);

    return result;
}

std::size_t solve_pattern_alt(const Pattern &pattern)
{
    // Can try accumulating distances. If accumulated distance > 1 it fails.
    std::size_t result = 0;

    int64_t old_row = find_row_reflection(pattern);
    int64_t old_col = find_col_reflection(pattern);

    int64_t row = find_row_reflection(pattern, old_row, 1);
    int64_t col = find_col_reflection(pattern, old_col, 1);

    result += (row+1) * 100;
    result += (col+1);

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


    std::vector<Pattern> patterns{};

    std::vector<std::string> pattern{};

    while ( std::getline(file, current_line) )
    {

        if ( current_line.length() < 2 ) {

            std::size_t height = pattern.size();
            std::size_t width = pattern[0].length();

            patterns.emplace_back(Pattern{width, height, std::move(pattern)});
            continue;
        }
        pattern.emplace_back(std::move(current_line));
    }

    std::size_t height = pattern.size();
    std::size_t width = pattern[0].length();

    patterns.emplace_back(Pattern{width, height, std::move(pattern)});

    std::size_t sum_1 = 0;
    std::size_t sum_2 = 0;


    // Part 1

    for ( auto &p: patterns )
    {
        sum_1 += solve_pattern(p);
    }

    for ( auto &p: patterns )
    {
        sum_2 += solve_pattern_alt(p);
    }


    assert(sum_1 == 39939);
    assert(sum_2 == 32069);

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << std::format("Result Part 1 {}\n", sum_1);
    std::cout << std::format("Result Part 2 {}\n", sum_2);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

