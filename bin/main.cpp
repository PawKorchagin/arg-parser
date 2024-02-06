#include <functional>

#include <iostream>
#include <numeric>

#include "lib/arg_parser.h"

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program");
    parser.AddIntArgument("--N").MultiValue(1).Positional().StoreValues(values);
    parser.AddFlag("-s", "--sum", "add args").StoreValue(opt.sum);
    parser.AddFlag("-m", "--mult", "multiply args").StoreValue(opt.mult);
    parser.AddHelp("Program accumulate arguments");
    
    if (!parser.Parse(argc, argv)) {
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    if (parser.Help()) {
        std::cout << parser.HelpDescription() << std::endl;
        return 0;
    }

    if (opt.sum) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
    } else if (opt.mult) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<>()) <<
            std::endl;
    } else {
        std::cout << "No one options had chosen" << std::endl;
        std::cout << parser.HelpDescription();
    }

    return 0;
}
