#include <gtest/gtest.h>
#include <sstream>

#include "arg_parser.h"

using namespace ArgumentParser;

std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

TEST(ArgParserTestSuite, EmptyTest) {
    ArgParser parser("My Empty Parser");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
}

TEST(ArgParserTestSuite, StringTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("--param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(parser.GetStringValue("--param1"), "value1");
}

TEST(ArgParserTestSuite, ShortNameTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("-p", "--param1", "");

    ASSERT_TRUE(parser.Parse(SplitString("app -p value1")));
    ASSERT_EQ(parser.GetStringValue("--param1"), "value1");
}

TEST(ArgParserTestSuite, DefaultStringTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("--param1").Default("value1");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetStringValue("--param1"), "value1");
}

TEST(ArgParserTestSuite, DefaultIntTest) {
    ArgParser parser("My Parser");
    parser.AddIntArgument("--param1").Default(5);

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetIntValue("--param1"), 5);
}

TEST(ArgParserTestSuite, DefaultFlagTest) {
    ArgParser parser("My Parser");
    parser.AddFlag("--param1").Default(true);

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetFlag("--param1"), true);
}

//TEST(ArgParserTestSuite, NoDefaultTest) {
//    ArgParser parser("My Parser");
//    parser.AddStringArgument("param1");
//
//    ASSERT_FALSE(parser.Parse(SplitString("app")));
//}

TEST(ArgParserTestSuite, StoreValueTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddStringArgument("--param1").StoreValue(value);
    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(value, "value1");
}

TEST(ArgParserTestSuite, MultiStringTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddStringArgument("--param1").StoreValue(value);
    parser.AddStringArgument("-a", "--param2", "some text");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1 --param2=value2")));
    ASSERT_EQ(value, "value1");
    ASSERT_EQ(parser.GetStringValue("--param2"), "value2");
}

TEST(ArgParserTestSuite, IntTest) {
    ArgParser parser("My Parser");
    parser.AddIntArgument("--param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=100500")));
    ASSERT_EQ(parser.GetIntValue("--param1"), 100500);
}

TEST(ArgParserTestSuite, MultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    parser.AddIntArgument("-p", "--param1", "something").MultiValue().StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(int_values[0], 1);
    ASSERT_EQ(int_values[1], 2);
    ASSERT_EQ(int_values[2], 3);
}

TEST(ArgParserTestSuite, SameArgmentsTest) {
    ArgParser parser("My Parser");
    parser.AddFlag("--param1");
    parser.AddStringArgument("--param1");
    ASSERT_FALSE(parser.Parse(SplitString("app --param1")));
}

// TEST(ArgParserTestSuite, MinCountMultiValueTest) {
//     ArgParser parser("My Parser");
//     std::vector<int> int_values;
//     size_t min_args_count = 10;
//     parser.AddIntArgument('p', "param1").MultiValue(min_args_count).StoreValues(int_values);
//
//     ASSERT_FALSE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
// }

TEST(ArgParserTestSuite, FlagTest) {
    ArgParser parser("My Parser");
    parser.AddFlag("-f", "--flag1", "this is flag argument for test");

    ASSERT_TRUE(parser.Parse(SplitString("app --flag1")));
    ASSERT_TRUE(parser.GetFlag("--flag1"));
}

TEST(ArgParserTestSuite, RepeatFlag) {
    ArgParser parser("My Parser");
    parser.AddFlag("--flag1");
    ASSERT_TRUE(parser.Parse(SplitString("app --flag1 --flag1")));
}

TEST(ArgParserTestSuite, FlagsTest) {
    ArgParser parser("My Parser");
    bool flag3;
    parser.AddFlag("-a", "--flag1", "flag1");
    parser.AddFlag("-b", "--flag2", "flag2").Default(true);
    parser.AddFlag("-c", "--flag3", "flag3").StoreValue(flag3);

    ASSERT_TRUE(parser.Parse(SplitString("app -a -c")));
    ASSERT_TRUE(parser.GetFlag("--flag1"));
    ASSERT_TRUE(parser.GetFlag("--flag2"));
    ASSERT_TRUE(flag3);
}

TEST(ArgParserTestSuite, SyntaxSugarIntTest) {
    ArgParser parser("Sugar parser");
    int some_value;
    parser.AddFlag("-a", "--flag1", "flag1");
    parser.AddFlag("-b", "--flag2", "flag2").Default(true);
    parser.AddIntArgument("-c", "--param", "param").StoreValue(some_value);
    ASSERT_TRUE(parser.Parse(SplitString("app -ac 5")));
    ASSERT_TRUE(parser.GetFlag("--flag1"));
    ASSERT_TRUE(parser.GetFlag("--flag2"));
    ASSERT_EQ(some_value, 5);
}

TEST(ArgParserTestSuite, SyntaxSugarStringTest) {
    ArgParser parser("Sugar parser");
    std::string some_value;
    parser.AddFlag("-a", "--flag1", "flag1");
    parser.AddFlag("-b", "--flag2", "flag2").Default(true);
    parser.AddStringArgument("-c", "--param", "param").StoreValue(some_value);
    ASSERT_TRUE(parser.Parse(SplitString("app -ac text4")));
    ASSERT_TRUE(parser.GetFlag("--flag1"));
    ASSERT_TRUE(parser.GetFlag("--flag2"));
    ASSERT_EQ(some_value, "text4");
}

TEST(ArgParserTestSuite, WrongArgumentTest) {
    ArgParser parser("My parser");
    parser.AddStringArgument("--unused-arg");
    ASSERT_FALSE(parser.Parse(SplitString("app --exist? no")));
}

TEST(ArgParserTestSuite, PostionalArgTest) {
    ArgParser parser("My Parser");
    int value;
    parser.AddIntArgument("--Param1").Positional().StoreValue(value);

    ASSERT_TRUE(parser.Parse(SplitString("app 1")));
    ASSERT_EQ(value, 1);
}

TEST(ArgParserTestSuite, PositionalMultiValueArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.AddIntArgument("--Param1").MultiValue(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5")));
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}

TEST(ArgParserTestSuite, HelpTest) {
    ArgParser parser("My Parser");
    parser.AddHelp("Some Description about program");

    ASSERT_TRUE(parser.Parse(SplitString("app --help")));
    ASSERT_TRUE(parser.Help());
}

TEST(ArgParserTestSuite, DoubleSameTypePositionalArgumentTest) {
    ArgParser parser("My parser");
    std::vector<int> values;
    int value;
    bool sum;

    parser.AddIntArgument("--pos-str").MultiValue().Positional().StoreValues(values);
    parser.AddIntArgument("--pos-str-single").Positional().StoreValue(value);
    parser.AddFlag("--sum").StoreValue(sum);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5 --sum")));
    ASSERT_EQ(sum, true);
    if (sum) {
        int res = 0;
        for (auto& i : values)
            res += i;

        ASSERT_EQ(res, 0);
        ASSERT_EQ(value, 5);
    }
}

// TEST(ArgParserTestSuite, HelpStringTest) {
//     ArgParser parser("My Parser");
//     parser.AddHelp("Some Description about program");
//     parser.AddStringArgument("-i", "--input", "File path for input file").MultiValue(1);
//     parser.AddFlag("-s", "--flag1", "Use some logic").Default(true);
//     parser.AddFlag("-p", "--flag2", "Use some logic");
//     parser.AddIntArgument("--number", "Some Number");
//     parser.AddStringArgument("--pos-int", "string positional argument").MultiValue().Positional();
//     parser.AddIntArgument("--pos-str").Positional();
//
//     ASSERT_TRUE(parser.Parse(SplitString("app --help")));
//     ASSERT_EQ(
//         parser.HelpDescription(),
//         "My Parser\n"
//         "Some Description about program\n"
//         "\n"
//         "    --number=<int>, Some Number\n"
//         "-i, --input=<string>, File path for input file [repeated, min args = 1]\n"
//         "-p, --flag2, Use some logic\n"
//         "-s, --flag1, Use some logic [default = true]\n"
//         "\n"
//         "-h, --help Display this help and exit\n"
//     );
// }
