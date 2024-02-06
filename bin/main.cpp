#include <functional>
#include <lib/arg_parser.h>

#include <iostream>
#include <numeric>

#include "unordered_map"

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("My Parser");
    parser.AddHelp("Some Description about program");
    parser.AddStringArgument("-i", "--input", "File path for input file").MultiValue(1);
    parser.AddFlag("-s", "--flag1", "Use some logic").Default(true);
    parser.AddFlag("-p", "--flag2", "Use some logic");
    parser.AddIntArgument("--number", "Some Number");
    parser.AddStringArgument("--pos-int", "string positional argument").MultiValue().Positional();
    parser.AddIntArgument("--pos-str").Positional();

    std::cout << parser.HelpDescription() << '\n';

    // Options opt;
    // std::vector<int> values;

    // ArgumentParser::ArgParser parser("Program");
    // // parser.AddIntArgument("--N").MultiValue(1).Positional().StoreValues(values);
    // // parser.AddFlag("-s", "--sum", "add args").StoreValue(opt.sum);
    // // parser.AddFlag("-m", "--mult", "multiply args").StoreValue(opt.mult);
    // parser.AddHelp("Program accumulate arguments");
    // parser.AddStringArgument("-p", "--param1", "param1");
    // if (!parser.Parse(argc, argv)) {
    //     std::cout << parser.HelpDescription() << std::endl;
    //     return 1;
    // }

    // std::cout << parser.GetStringValue("--param1") << '\n';

    // //    if(parser.Help()) {
    // //        std::cout << parser.HelpDescription() << std::endl;
    // //        return 0;
    // //    }
    //
    // if (opt.sum) {
    //     std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
    // } else if (opt.mult) {
    //     std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) <<
    //         std::endl;
    // } else {
    //     std::cout << "No one options had chosen" << std::endl;
    //     std::cout << parser.HelpDescription();
    // }

    return 0;
}
