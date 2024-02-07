# arg-parser

![](https://app.gemoo.com/share/image-annotation/613614667541118976?codeId=DWaL9zR25W7kp&origin=imageurlgenerator&card=613614665079062528)

## Table of Contents
- [Short Description](#short-description)
- [Quick Start](#quick-start)
- [Add Typed Argument](#add-typed-argument)
    - [Integer Argument](#integer-argument)
    - [String Argument](#string-argument)
    - [Flag](#flag)
- [Call Parsing](#call-parsing)
- [Get Argument Value From Command Line](#get-argument-value-from-command-line)
- [Storing Argument Value](#storing-argument-value)
- [MultiValue Argument](#multivalue-argument)
- [Positional Argument](#positional-argument)
- [Default Argument](#default-argument)
- [Help](#help)
- [Other Shortcuts](#other-shortcuts)
- [Currently Under Development](#currently-under-development)

## Short Description 

In your project, you...<br>
Need to parse arguments from the command line? Don't use exceptions? This class will help you to save time.

## Quick Start

Include class library or manually add class-files *arg_parser.cpp* and *arg_parser.h* from *lib* directory to your project.

```cpp
#include <arg-parser/lib/arg_parser.h>
```

To start parsing command line arguments, create an ```ArgParser``` parser object.

```cpp
ArgumentParser::ArgParser parser("My parser");
```

## Add Typed Argument

You can configure set of options for your project by adding them to parser.

### Integer Argument
```c++
parser.AddIntArgument("-i", "--int-argument", "your int argument from command line"); //full append method
parser.AddIntArgument("--int-argument"); //shortcut
```
### String Argument 
```c++
parser.AddStringArgument("-s", "--string-argument", "your string argument from command line"); //full append method
parser.AddStringArgument("--string-argument"); //shortcut
```
### Flag 
```c++
parser.AddFlag("-f", "--flag", "your flag from command line"); // full append method 
parser.AddFlag("--flag") // shortcut
```

## Call Parsing

To parse the command line arguments, simply call the Parse() method on your ArgParser object.

```c++
parser.Parse(argc, argv);
```

Also, you can put ```std::vector<std::string>``` as parameter.

## Get Argument Value From Command Line

You can retrieve the value of a specific argument from the command line using ```GetIntValue("arg")``` or ```GetStringValue("arg")``` or ```GetFlag("arg")``` for flags.

```c++
ArgParser parser("My parser");
parser.AddIntArgument("--int-arg");
parser.AddStringArgument("--string-arg");
parser.AddFlag("--flag");

if (!parser.Parse(argc, argv)) {
    std::cout << parser.HelpDescription() << std::endl;
    return 1;
}

if (parser.Help()) {
    std::cout << parser.HelpDescription() << std::endl;
    return 0;
}

std::cout << parser.GetIntValue("--int-arg") << std::endl;
std::cout << parser.GetStringValue("--string-arg") << std::endl;
std::cout << parser.GetFlag("--flag") << std::endl;
```

Execute program with following options

```console
foo@bar:/example/directories/$ ./main --int-arg=5 --string-arg=this_is_string --flag
```

You can view value of these arguments

```text
5
```

## Storing Argument Value

After defining the parser and specifying the arguments, you can utilize the ```StoreValue()```
method to capture the value of an argument and store it into a variable within your program.

```c++
ArgParser parser("My Parser");

std::string value;

parser.AddStringArgument("--param1").StoreValue(value)
parser.Parse(argc, argv)
std::cout << value << std::endl;
```

Run this code

```console
```console
foo@bar:/example/directories/$ ./main --param1=val
```

Result:

```text
val
```

## MultiValue Argument

MultiValue arguments allow you to specify multiple values for a single option. You just need add ```MultiValue()``` after adding argument.


```c++
ArgParser parser("My Parser");
std::vector<int> int_values;
parser.AddIntArgument("-p", "--param1", "something").MultiValue().StoreValues(int_values);
parser.Parse(argc, argv);
for (const auto& value : int_values) {
    std::cout << value << ' ';
}
```

Execute this code with multi value option

```console
foo@bar:/example/directories/$ ./main --param1=1 --param1=2 --param1=3
```

Result:

```text
1 2 3
```

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

Running the code to calculate the sum of numbers from command-line:

```console
foo@bar:/example/directories/$ ./main 1 2 3 4 5 --sum
15
```

## Default Argument

Some arguments may not appear on the command line? You can set default value for them and don't worry about errors.

```c++
ArgParser parser("My Parser");

std::string value;

parser.AddStringArgument("--param1").Default("default value")
parser.Parse(argc, argv)
std::cout << value << std::endl;
```

Run this code

```console
```console
foo@bar:/example/directories/$ ./main
```

Result:

```text
default value
```

### Note
If you don't use non-default argument in command line ```Parse(argc, argv)``` will return false.
## Help

To add help functionality to your argument parser, you can use the ```AddHelp("desc")```
method provided by your ArgParser class. This method allows you to specify
a description for your program. When the user requests help by including
the appropriate flag in the command line (-h, --help),
your parser should display this description along with information about the available options.

```c++
parser.AddHelp("Program short description");
```

To show the information about your program you need to call ```parser.HelpDescription()```.

For example:

```c++
ArgParser parser("My Parser");
parser.AddHelp("Some Description about program");
parser.AddStringArgument("-i", "--input", "File path for input file").MultiValue(1);
parser.AddFlag("-s", "--flag1", "Use some logic").Default(true);
parser.AddFlag("-p", "--flag2", "Use some logic");
parser.AddIntArgument("--number", "Some Number");
parser.AddStringArgument("--pos-int", "string positional argument").MultiValue().Positional();
parser.AddIntArgument("--pos-str").Positional();

if (!parser.Parse(argc, argv)) {
    std::cout << parser.HelpDescription() << std::endl;
    return 1;
}

if (parser.Help()) {
    std::cout << parser.HelpDescription() << std::endl;
    return 0;
}
```

Running this code...

```console
foo@bar:/example/directories/$ ./main --help
```

And you can view detailed description about using arguments

```text
My Parser
Some Description about program

    --number=<int>, Some Number
    --pos-str=<int>, [positional]
-i, --input=<string>, File path for input file [repeated]
    --pos-int=<string>, string positional argument [repeated, positional]
-s, --flag1, Use some logic [default = true]
-p, --flag2, Use some logic
-h, --help, Display this help and exit
```

## Other shortcuts

You can use shorthand variant of use arguments in command line.

```c++
int some_value;

ArgParser parser("My Parser");
parser.AddFlag("-a", "--flag1", "flag1");
parser.AddFlag("-b", "--flag2", "flag2").Default(true);
parser.AddIntArgument("-c", "--param", "param").StoreValue(some_value);
parser.Parse(argc, argv);
std::cout << parser.GetFlag("--flag1") << std::endl;
std::cout << parser.GetFlag("--flag2) << std::endl;
std::cout << some_value << std::endl;
```

Use special shorthand type in command line

```console
```console
foo@bar:/example/directories/$ ./main -ac 5
```

Result:

```text
1
1
5
```

## Currently Under Development

- ```branch dev``` MultiValue GetValue(index)
- ```branch dev``` --multivalue-arg 1 2 3 4 ...
- ```branch dev``` MultiValue GetSize(multivalue-arg)
