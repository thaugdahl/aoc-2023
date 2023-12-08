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

using Graph = std::unordered_map<std::string, std::pair<std::string, std::string>>;

std::size_t solve_p1(const Graph &graph, const std::vector<int> &instructions, const std::string &start)
{
    std::string current = start;

    std::size_t res = 0;

    while ( true )
    {
        for ( auto i : instructions )
        {

            if ( current.compare("ZZZ") == 0 )
                return res;

            if ( ! graph.contains(current) ) {
                throw "Key not found!";
            }

            auto [l, r] = graph.at(current);

            if ( i == 0 ) current = l;
            else current = r;

            res++;
        }
    }

    return res;

}

std::size_t solve_p2(const Graph &graph, const std::vector<int> &instructions, const std::string &start)
{
    std::string current = start;

    std::size_t res = 0;

    while ( true )
    {
        for ( auto i : instructions )
        {

            if ( current.ends_with('Z') )
                return res;

            if ( ! graph.contains(current) ) {
                throw "Key not found!";
            }

            auto [l, r] = graph.at(current);

            if ( i == 0 ) current = l;
            else current = r;

            res++;
        }
    }

    return res;

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

    std::vector<int> instructions;
    Graph graph;
    std::vector<std::string> labels;

    std::string current_line;

    MicrosecondClock clock;


    clock.start();

    std::size_t sum = 0;

    std::getline(file, current_line);

    for ( auto &c : current_line )
    {
        if ( c == 'L' ) instructions.emplace_back(0);
        else if ( c == 'R' ) instructions.emplace_back(1);
    }


    std::getline(file, current_line);

    while ( std::getline(file, current_line) )
    {
        std::string label = current_line.substr(0, 3);
        std::string left = current_line.substr(7, 3);
        std::string right = current_line.substr(12,3);

        graph.emplace(std::make_pair(label, std::make_pair(std::move(left), std::move(right))));

        if ( label.ends_with('A') ) labels.emplace_back(label);
    }

    auto res = solve_p1(graph, instructions, "AAA");

    std::size_t LCM = 1;

    for ( const auto &l:  labels )
    {
        LCM = std::lcm(LCM, solve_p2(graph, instructions, l));
    }


    std::cout << std::format("Part 1: {}\n", res);
    std::cout << std::format("Part 2: {}\n", LCM);

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

