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

void tests()
{
}

using HistoryDataType = int64_t;
using History = std::vector<HistoryDataType>;

History get_history(const std::string &line)
{

    std::istringstream ss{line};

    HistoryDataType current_value{0};

    std::vector<HistoryDataType> data;

    while ( ss >> current_value )
    {
        data.emplace_back(current_value);
    }

    return data;

}

History generate_diff_history(const History &history)
{
    History result;


    auto trailing = history.begin();
    auto leading = history.begin() + 1;

    while ( leading != history.end() )
    {
        auto diff = *leading - *trailing;
        result.emplace_back(diff);

        ++leading;
        ++trailing;
    }

    return result;
}

HistoryDataType extrapolate_next(const History &history)
{

    std::size_t num_sequences = 0;

    std::vector<History> sequences{};

    bool all_zero = false;

    sequences.emplace_back(generate_diff_history(history));


    while ( ! all_zero )
    {
        auto diff_history = generate_diff_history(sequences.back());

        all_zero = std::all_of(diff_history.begin(), diff_history.end(), [] (auto &x) {
               return x == 0;
            });

        if ( all_zero ) break;

        sequences.emplace_back(std::move(diff_history));
    }

    auto bottom = std::rbegin(sequences);
    auto leading = std::rbegin(sequences) + 1;

    auto last = 0;
    while ( leading != std::rend(sequences) )
    {
        (*leading).emplace_back((*leading).back() + (*bottom).back());
        ++leading;
        ++bottom;
    }

    return sequences[0].back() + history.back();
}


HistoryDataType extrapolate_prev(const History &history)
{

    std::size_t num_sequences = 0;

    std::vector<History> sequences{};

    sequences.emplace_back(generate_diff_history(history));

    bool all_zero = false;

    while ( ! all_zero )
    {
        auto diff_history = generate_diff_history(sequences.back());


        all_zero = std::all_of(diff_history.begin(), diff_history.end(), [] (auto &x)
            {
               return x == 0;
            });

        sequences.emplace_back(std::move(diff_history));


    }

    auto iter = std::rbegin(sequences);

    auto first = 0;
    while ( iter != std::rend(sequences) )
    {
        (*iter).emplace_back((*iter).front() - first);
        first = (*iter).back();
        ++iter;
    }

    return history.front() - sequences[0].back();
}

int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);

    // Test runner
    tests();

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

    std::size_t sum_fw = 0;
    std::size_t sum_bw = 0;


    while ( std::getline(file, current_line) )
    {
        auto history = get_history(current_line);



        sum_fw += extrapolate_next(history);
        sum_bw += extrapolate_prev(history);
    }




    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << std::format("Sum of forward extrapolated values {}\n", sum_fw);
    std::cout << std::format("Sum of backward extrapolated values {}\n", sum_bw);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

