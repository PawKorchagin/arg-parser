#include <sstream>
#include <charconv>

#include "arg_parser.h"

// namespace {
void PrintError(const std::string msg, const std::string spec) {
    std::cerr << "Error: " << msg << ' ' << spec << '\n';
}

void PrintWarning(const std::string msg) {
    std::cerr << "Warning: " << msg << '\n';
}

namespace ArgumentParser {
ArgParser::ArgParser(const std::string name) : program_name_(name) {
}

bool IsFlagArgument(const FlagConfig& args, const std::string& arg) {
    return args.Contains(arg) || args.KeyContains(arg);
}

bool IsIntArgument(const IntArgumentConfig& args, const std::string& arg) {
    return args.Contains(arg) || args.KeyContains(arg);
}

bool IsStringArgument(const StringArgumentConfig& args, const std::string& arg) {
    return args.Contains(arg) || args.KeyContains(arg);
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        // PrintError("No arguments in program run configuration");
        return true;
    }

    for (size_t i = 1 ; i < args.size() ; ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cerr << HelpDescription();
            return true;
        }

        std::string arg = args[i].substr(0, args[i].find('='));
        std::string value = args[i].substr(args[i].find('=') + 1);

        if (flags_.Contains(arg) || flags_.KeyContains(arg)) {
            *flags_.GetValue(arg) = true;
        } else if (str_args_.Contains(arg) || str_args_.KeyContains(arg)) {
            // const auto stored = str_args_.GetValue(arg);
            if (arg == args[i]) {
                arg = str_args_.GetByKey(arg);
                ++i;
                if (i < args.size()) {
                    value = args[i];
                } else {
                    //TODO default
                    PrintWarning("Non-default argument missing value");
                    return false;
                }
            }

            if (str_args_.IsMultiValueArgument(arg)) {
                if (str_args_.IsStored(arg)) {
                    str_args_.GetValues(arg)->push_back(value);
                } else {
                    //TODO create multivalue
                }
            } else {
                if (str_args_.IsStored(arg)) {
                    *str_args_.GetValue(arg) = value;
                } else {
                    str_args_.CreateValue(arg, value);
                }
            }
        } else if (int_args_.Contains(arg) || int_args_.KeyContains(arg)) {
            int res;
            if (arg == args[i]) {
                ++i;
                if (i < args.size()) {
                    auto [_, ec] = std::from_chars(args[i].data(), args[i].data() + args[i].size(), res);
                    if (ec == std::errc::invalid_argument) {
                        PrintWarning("Given string instead int positional argument");
                        return false;
                    }
                    if (ec == std::errc::result_out_of_range) {
                        PrintWarning("Given number more than an int");
                        return false;
                    }
                } else {
                    //TODO default
                    PrintWarning("Non-default argument missing value");
                    return false;
                }
            } else {
                auto [_, ec] = std::from_chars(args[i].data(), args[i].data() + args[i].size(), res);
                if (ec == std::errc::invalid_argument) {
                    PrintWarning("Not a number given as int argument");
                    return false;
                } else if (ec == std::errc::result_out_of_range) {
                    PrintWarning("Given number more than an int");
                    return false;
                }
            }
            if (int_args_.IsMultiValueArgument(arg)) {
                if (int_args_.IsStored(arg)) {
                    int_args_.GetValues(arg)->push_back(res);
                } else {
                    //TODO create multivalue
                }
            } else {
                if (int_args_.IsStored(arg)) {
                    *int_args_.GetValue(arg) = res;
                } else {
                    int_args_.CreateValue(arg, res);
                }
            }
        } else if (int result = 0 ; std::from_chars(args[i].data(), args[i].data() + args[i].size(), result).ec ==
            std::errc{}
            && int_args_
            .IsPositional()) {
            if (int_args_.IsStored(int_args_.GetPositional()))
                int_args_.GetValues(int_args_.GetPositional())->push_back(result);
            else {
                //TODO
            }
        } else if (str_args_.IsPositional()) {
            if (str_args_.IsStored(str_args_.GetPositional()))
                str_args_.GetValues(str_args_.GetPositional())->push_back(args[i]);
            else {
                //TODO
            }
        }
    }

    return true;
}

bool ArgParser::Parse(int argc, char** argv) {
    return Parse({argv, argv + argc});
}

ArgParser& ArgParser::AddHelp(const std::string& desc) {
    is_added_help_ = true;
    return AddFlag("-h", "--help", desc);
}

ArgParser& ArgParser::AddFlag(const std::string& key,
                              const std::string& name,
                              const std::string& desc) {
    cur_arg_ = name;
    flags_.SetArgument(key, name, desc);
    return *this;
}

ArgParser& ArgParser::AddFlag(const std::string& name, const std::string& desc) {
    return AddFlag("", name, desc);
}

ArgParser& ArgParser::StoreValue(bool& value) {
    flags_.PutValue(cur_arg_, &value);
    return *this;
}

std::string ArgParser::HelpDescription() const {
    std::stringstream out;

    if (!is_added_help_) {
        out << "No info had provided yet";
    }

    return out.str();
}

ArgParser& ArgParser::AddStringArgument(const std::string& name,
                                        const std::string& desc) {
    return AddStringArgument("", name, desc);
}

ArgParser& ArgParser::AddStringArgument(const std::string& key,
                                        const std::string& name,
                                        const std::string& desc) {
    cur_arg_ = name;
    str_args_.SetArgument(key, name, desc);
    return *this;
}

ArgParser& ArgParser::StoreValue(std::string& value) {
    str_args_.PutValue(cur_arg_, &value);
    return *this;
}
ArgParser& ArgParser::MultiValue(uint min_count) {
    str_args_.MakeMulti(cur_arg_);
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<std::string>& values) {
    str_args_.PutValues(cur_arg_, &values);
    return *this;
}
ArgParser& ArgParser::Positional() {
    // std::cerr << cur_arg_ << '\n';
    if (IsStringArgument(str_args_, cur_arg_)) {
        str_args_.MakeMulti(cur_arg_);
        str_args_.PutPositional(cur_arg_);
    } else if (IsIntArgument(int_args_, cur_arg_)) {
        int_args_.MakeMulti(cur_arg_);
        int_args_.PutPositional(cur_arg_);
    }

    return *this;
}

std::string& ArgParser::GetStringValue(const std::string& name) {
    return *str_args_.GetValue(name);
}

int& ArgParser::GetIntValue(const std::string& name) {
    return *int_args_.GetValue(name);
}

ArgParser& ArgParser::AddIntArgument(const std::string& key,
                                     const std::string& name,
                                     const std::string& desc) {
    cur_arg_ = name;
    int_args_.SetArgument(key, name, desc);
    return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& name,
                                     const std::string& desc) {
    return AddIntArgument("", name, desc);
}

ArgParser& ArgParser::StoreValue(int& value) {
    int_args_.PutValue(cur_arg_, &value);
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<int>& values) {
    int_args_.PutValues(cur_arg_, &values);
    return *this;
}

ArgParser::~ArgParser() = default;

std::string BaseArgumentConfig::GetByKey(const std::string& key) {
    return keys_[key];
}

std::string BaseArgumentConfig::GetDescription(const std::string& name) {
    return desc_[name];
}

bool BaseArgumentConfig::KeyContains(const std::string& key) const {
    return keys_.contains(key);
}

void BaseArgumentConfig::SetArgument(const std::string& key,
                                     const std::string& name,
                                     const std::string& desc) {
    used_.insert(name);
    keys_.insert({key, name});
    desc_.insert({name, desc});
}

void BaseArgumentConfig::MakeMulti(const std::string& arg) {
    is_multi_.insert(arg);
}

bool BaseArgumentConfig::IsMultiValueArgument(const std::string& arg) const {
    return is_multi_.contains(arg);
}

void StringArgumentConfig::PutValue(const std::string& name, std::string* value) {
    names_.insert({name, value});
}

bool BaseArgumentConfig::Contains(const std::string& name) const {
    return used_.contains(name);
}

std::string*& StringArgumentConfig::GetValue(const std::string& name) {
    if (!names_.contains(name)) {
        PrintError("No such argument in parser:", name);
        exit(EXIT_FAILURE);
    }
    return names_[name];
}

void StringArgumentConfig::PutValues(const std::string& name,
                                     std::vector<std::string>* values) {
    multi_.insert({name, values});
}

void StringArgumentConfig::PutPositional(const std::string& arg) {
    positional_ = arg;
    is_positional = true;
}

std::string StringArgumentConfig::GetPositional() const {
    return positional_;
}

bool StringArgumentConfig::IsPositional() const {
    return is_positional;
}

void StringArgumentConfig::CreateValue(const std::string& name, const std::string& value) {
    cvalue_.insert({name, value});
    names_.insert({name, &cvalue_[name]});
}

bool StringArgumentConfig::IsStored(const std::string& arg) const {
    return names_.contains(arg) || multi_.contains(arg);
}

// void StringArgumentConfig::CreateValues(std::string name, const std::string& value) {
//     if (!cvalues_.contains(name)) {
//         cvalues_.insert({name, {}});
//     }
//     cvalues_[name].push_back(value);
//     multi_.insert({name, })
// }

std::vector<std::string>*&
StringArgumentConfig::GetValues(const std::string& name) {
    return multi_[name];
}

void IntArgumentConfig::PutValue(const std::string& name, int* value) {
    names_.insert({name, value});
}

void IntArgumentConfig::PutValues(const std::string& name,
                                  std::vector<int>* values) {
    multi_.insert({name, values});
}
void IntArgumentConfig::PutPositional(const std::string& arg) {
    positional_ = arg;
    is_positional_ = true;
}

int*& IntArgumentConfig::GetValue(const std::string& name) {
    if (!names_.contains(name)) {
        PrintError("No such argument in parser:", name);
        exit(EXIT_FAILURE);
    }
    return names_[name];
}
std::vector<int>*& IntArgumentConfig::GetValues(const std::string& name) {
    return multi_[name];
}
std::string IntArgumentConfig::GetPositional() const {
    return positional_;
}
bool IntArgumentConfig::IsPositional() const {
    return is_positional_;
}
void IntArgumentConfig::CreateValue(const std::string& name, const int value) {
    cvalue_.insert({name, value});
    names_.insert({name, &cvalue_[name]});
}

bool IntArgumentConfig::IsStored(const std::string& arg) const {
    return names_.contains(arg) || multi_.contains(arg);
}

bool ArgParser::Help() const { return is_added_help_; }

void FlagConfig::PutValue(std::string name, bool* value) {
    names_.insert({name, value});
}
void FlagConfig::MakeMulti(const std::string& basic_string) {
    PrintWarning("try to make multivalue flag argument");
}

bool*& FlagConfig::GetValue(const std::string name) {
    if (!names_.contains(name)) {
        PrintError("No such argument in parser:", name);
        exit(EXIT_FAILURE);
    }
    return names_[name];
}
} // namespace ArgumentParser
