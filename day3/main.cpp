#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <string>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <utility>
#include <array>
#include <sstream>
#include <string_view>
#include <vector>
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

struct RawSchematic {
    RawSchematic(std::size_t width, std::size_t height) : width{width}, height{height}
    {
        data = std::vector<unsigned char>{};
        data.resize(width * height);
    }

    RawSchematic(RawSchematic &&other) = default;
    RawSchematic(const RawSchematic &other) = default;
    RawSchematic &operator=(RawSchematic &&other) = default;
    RawSchematic &operator=(const RawSchematic &other) = default;
    ~RawSchematic() = default;


    // Data Members
    std::size_t width;
    std::size_t height;

    std::vector<unsigned char> data;
};


struct Number
{
    int value;
    int col_begin;
    int col_end;
    int row;
};

struct Symbol
{
    int col;
    int row;
    char symbol_char;

    std::vector<Number> adjacent_numbers;
};

inline bool is_part_symbol(char c) noexcept {
    return c != '.' && ! std::isdigit(c);
}


std::optional<RawSchematic> get_raw_schematic(const std::filesystem::path &path)
{

    std::ifstream file{path};

    std::string current_line{};


    if ( ! std::getline(file, current_line) ) {
        return std::optional<RawSchematic>{};

    }

    // Get the number of rows and columns
    std::size_t line_length = current_line.length(); // Including newline
    file.seekg(0, file.end);
    std::size_t file_size = file.tellg();
    file.seekg(0, file.beg);

    std::size_t num_lines = file_size / line_length;

    RawSchematic raw_schematic{line_length, num_lines - 1};

    std::size_t i = 0;
    char c;
    while ( file.read(&c, 1) ) {
        if ( c == '\n' ) continue;
        raw_schematic.data[i++] = c;
        // if ( c == '\n' ) std::cerr << "NEWLINE \n";

    }

    file.close();
    return std::optional<RawSchematic>{raw_schematic};

}


Number parse_integer(const RawSchematic &schematic, std::size_t x, std::size_t y)
{
    std::size_t offset = 0;


    int sum = 0;

    while ( x + offset < schematic.width && std::isdigit(schematic.data[y*schematic.width + x + offset]))
    {

        char c = schematic.data[y*schematic.width + x + offset] ;

        sum *= 10;
        sum +=  c - '0';

        offset++;
    }


    return Number{sum, static_cast<int>(x), static_cast<int>(x + offset - 1), static_cast<int>(y)};

}

int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);

    if ( ! std::filesystem::exists(inputFile) ) {
        std::cerr << "Input file does not exist\n";
        exit(EXIT_FAILURE);
    };


    MicrosecondClock clock;

    clock.start();


    auto raw_schematic_option = get_raw_schematic(inputFile);
    if ( ! raw_schematic_option.has_value() ) {
        exit(1);
    }

    // auto raw_schematic = raw_schematic_option.value();

    RawSchematic schematic = raw_schematic_option.value();

    const auto width = schematic.width;
    const auto height = schematic.height;
    const auto &data = schematic.data;


    auto idx_1d = [width] (auto x, auto y) {
        return x + y*width;
    };


    std::vector<Number> numbers;
    std::vector<Symbol> symbols;


    for ( int y = 0; y < height; y++ )
    {

        int x = 0;
        while ( x < width )
        {
            char currentChar = data[idx_1d(x,y)];

            if (std::isdigit(currentChar))
            {

                Number res = parse_integer(schematic, x, y);
                numbers.emplace_back(res);

                // Advance by number of characters that comprises the number
                x += res.col_end - res.col_begin + 1;
            } else if ( is_part_symbol(currentChar) ) {
                symbols.emplace_back(Symbol{x, y, currentChar, {}});
                x++;
            } else {
                x++;
            }
        }
    }

    // To become part of a gear, it has to be within the bounding box
    // ...********..
    // ...*NUMBER*..
    // ...********..
    //
    // Meaning, (x,y) | y in [row - 1, row+1] && x in [col_begin - 1, col_end + 1]

    for ( auto &x : numbers )
    {
        for ( auto &y : symbols )
        {
            auto s_x = y.col;
            auto s_y = y.row;

            bool in_horizontal_boundary = s_x >= x.col_begin - 1 && s_x <= x.col_end + 1;
            bool in_vertical_boundary = s_y >= x.row - 1 && s_y <= x.row + 1;


            if ( in_horizontal_boundary && in_vertical_boundary ) {
                y.adjacent_numbers.push_back(x);
            }
        }
    }


    std::size_t sum = 0;

    for ( auto &y : symbols )
    {
        if ( y.symbol_char == '*' && y.adjacent_numbers.size() == 2 )
        {
            sum += y.adjacent_numbers[0].value * y.adjacent_numbers[1].value;
        }

    }



    std::size_t sum2 = 0;

    for ( auto &y: symbols )
    {
        for ( auto &num : y.adjacent_numbers)
        {
            sum2 += num.value;
        }
    }





    clock.stop();
    uint64_t duration_microseconds = clock.elapsed();

    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Sum2: " << sum2 << std::endl;
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

