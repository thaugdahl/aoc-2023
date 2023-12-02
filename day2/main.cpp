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

std::vector<std::string_view> split_string(const std::string_view &string, const std::string &delim) {

    std::vector<std::string_view> res;

    std::string_view view{string};

    while ( ! view.empty() )
    {
        std::size_t pos = view.find(delim);


        res.emplace_back(view.substr(0, pos));
        view = view.substr(pos+delim.length());

        if ( pos == std::string_view::npos) break;
    }

    return res;
}

using uint = unsigned int;

using Draw = std::tuple<uint, uint, uint>;
using Game = std::pair<uint, std::vector<Draw>>;


void print_game(const Game &game)
{
    std::cout << "Game " << game.first << ": ";
    for ( auto &x : game.second )
    {
        auto [r,g,b] = x;
        std::cout << static_cast<int>(r) << ", " << static_cast<int>(g) << ", " << static_cast<int>(b) << " ; ";
    }

    std::cout << "\n";
}

Game make_game_from_string(const std::string &game_string)
{
    Game result;
    auto id_and_game = split_string(game_string, ": ");
    auto id_str = id_and_game[0];

    auto game_str = id_and_game[1];

    auto draw_strings = split_string(game_str, "; ");


    uint16_t game_id = std::stoi(std::string{id_str.substr(id_str.find(" "))});


    std::vector<Draw> draws;

    std::transform(std::begin(draw_strings), std::end(draw_strings), std::back_inserter(draws), [] (std::string_view draw_string) {

        auto sets = split_string(draw_string, ", ");

        int red = 0, green = 0, blue = 0;
        for ( auto &x : sets ) {
            auto num_color = split_string(x, " ");
            int num = std::stoi(std::string{num_color[0]});
            auto color_string = num_color[1];

            if ( color_string == "red" ) {
                red = num;
            } else if ( color_string == "green" ) {
                green = num;
            } else if ( color_string == "blue" ) {
                blue = num;
            } else {}
        }


        return std::make_tuple(red, green, blue);
    });

    result.first = game_id;
    result.second = draws;
    return result;
}


static constexpr unsigned int  MAX_RED = 12;
static constexpr unsigned int  MAX_GREEN = 13;
static constexpr unsigned int  MAX_BLUE = 14;

bool verify_game(const Game &game)
{
    bool valid = true;

    for ( auto &x : game.second ) {
        auto [r,g,b] = x;

        if ( r > MAX_RED ) valid = false;
        if ( g > MAX_GREEN ) valid = false;
        if ( b > MAX_BLUE ) valid = false;
    }

    return valid;
}


int power_of_game(const Game &game)
{

    uint max_red = 0;
    uint max_green = 0;
    uint max_blue = 0;

    for ( auto &x : game.second )
    {
        auto [r,g,b] = x;

        max_red = std::max(max_red, r);
        max_green = std::max(max_green, g);
        max_blue = std::max(max_blue, b);
    }

    return max_red * max_green * max_blue;
}

void tests()
{

    {
        std::string testString{"Game 2: XXX"};

        std::vector<std::string_view> strings = split_string(testString, ": ");

        assert(strings.size() == 2);

        TEST_TRUE(strings.size() == 2);
    }

    {
        std::string testString{"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"};

        std::string_view game_str;
        std::string_view rhs;
        {
            std::vector<std::string_view> strings = split_string(testString, ": ");
            game_str = strings[0];
            rhs = strings[1];
        }

        auto testStrings = split_string(game_str, " ");
        TEST_TRUE(testStrings[1] == "1");
    }

    {
        std::string testString{"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"};

        auto strings = split_string(testString, ": ");
        std::string_view game_str = strings[0];
        std::string_view rhs = strings[1];

        auto draws = split_string(rhs, "; ");
        TEST_TRUE(draws.size() == 3);


        auto first_draw = draws[0];
        auto sets = split_string(first_draw, ", ");

        auto first_set = sets[0];


        auto num_color = split_string(first_set, " ");
        TEST_TRUE(num_color[0] == "3");
    }

    {
        Game testGame = make_game_from_string("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green");

        TEST_TRUE(testGame.first == 1);

        TEST_TRUE(testGame.second.size() == 3);

        auto &draws = testGame.second;

        {
        auto [r, g, b] = draws[0];

        TEST_TRUE(r == 4);
        TEST_TRUE(g == 0);
        TEST_TRUE(b == 3);
        }

        {
        auto [r, g, b] = draws[1];

        TEST_TRUE(r == 1);
        TEST_TRUE(g == 2);
        TEST_TRUE(b == 6);
        }


        {
        auto [r, g, b] = draws[2];

        TEST_TRUE(r == 0);
        TEST_TRUE(g == 2);
        TEST_TRUE(b == 0);
        }
    }

    {
        Game game1 = make_game_from_string (   "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green" );
        Game game2 = make_game_from_string (   "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue" );
        Game game3 = make_game_from_string (   "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red" );
        Game game4 = make_game_from_string (   "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red" );
        Game game5 = make_game_from_string (   "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green" );

        auto power = power_of_game(game1);
        TEST_TRUE(power == 48);

        power = power_of_game(game2);
        TEST_TRUE(power == 12);
        power = power_of_game(game3);
        TEST_TRUE(power == 1560);
        power = power_of_game(game4);
        TEST_TRUE(power == 630);
        power = power_of_game(game5);
        TEST_TRUE(power == 36);

    }

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

    int sum = 0;

    std::string current_line{};

    MicrosecondClock clock;

    clock.start();


    while ( std::getline(file, current_line) )
    {

        Game game = make_game_from_string(current_line);
        print_game(game);

        // if ( verify_game(game) ) {
        //    sum += game.first;
        // }

        sum += power_of_game(game);
    }

    clock.stop();
    uint64_t duration_microseconds = clock.elapsed();





    file.close();

    std::cout << "Final sum: " << sum << "\n";
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

