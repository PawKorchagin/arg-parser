# arg-parser

## Short Description 

In your project, you...<br>
Need to parse arguments from the command line? Don't use exceptions? This class will help you to save time.

### Functionality

- ```AddStringArgument("-s", "--string-argument", "your string argument from command line")```
- ```AddIntArgument("-i", "--int-argument", "your int argument from command line")```
- ```AddFlag("-f", "--flag", "your flag from command line")```

For further information view docs (TODO)

## Quick Start

Include class library or manually add class-files *arg_parser.cpp* and *arg_parser.h* from *lib* directory to your project.

```cpp
#include <arg-parser/lib/arg_parser.h>
```

To start parsing command-line arguments, create an ```ArgParser``` parser object.

```cpp
ArgumentParser::ArgParser parser("My parser");
```

## Add Parser Argument

TODO

## Add Help

TODO

## MultiValue Argument

TODO

## Get Argument Value From Command Line

TODO

## Storing Argument Value

TODO



## Positional Argument
### What is positional argument?
Positional argument allows you to avoid writing the name of the argument on the command line.

Currently, 2 types of positional arguments are implemented: int and string.

To create positional argument you need to use ```.Positional()``` method.

### Example

Here's an example of using ***positional argument***:
```c++
#include <iostream>
#include <functional>
#include <numeric>

#include "arg-parser/lib/arg_parser.h"

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
```

Running the code to multiply numbers from command-line: 

```console
foo@bar:/example/directories/$ ./program 1 2 3 4 5 --mult
120
```
Explanation:
```text
1 * 2 * 3 * 4 * 5 = 120
```

Running the code to calculate the sum of numbers from command-line:

```console
foo@bar:/example/directories/$ ./main 1 2 3 4 5 --sum
15
```

Explanation:
```text
1 + 2 + 3 + 4 + 5 = 15
```

## Default Argument