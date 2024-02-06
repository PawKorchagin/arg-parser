#include <sstream>
#include <charconv>
#include <algorithm>
#include <utility>
#include <iomanip>

#include "arg_parser.h"

// namespace {
void PrintError(const std::string_view msg, const std::string_view spec) {
    std::cerr << "Error: " << msg << ' ' << spec << '\n';
}

void PrintWarning(const std::string_view msg, const std::string_view spec = "") {
    std::cerr << "Warning: " << msg << ' ' << spec << '\n';
}

std::string MergeChars(const char a, const char b) {
    return std::string() + a + b;
}

namespace ArgumentParser {
ArgParser::ArgParser(std::string name) : program_name_(std::move(name)) {
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
    if (IsArgumentCoincidence())
        return false;

    for (size_t i = 1 ; i < args.size() ; ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cerr << HelpDescription();
            return true;
        } {
            std::vector<std::string> cur_arg_config;
            cur_arg_config.push_back(args[i]);
            if (i + 1 < args.size())
                cur_arg_config.push_back(args[i + 1]);

            size_t j = 0;
            const auto is_argument = this->IsArgument(cur_arg_config, j);

            i += j;

            if (is_argument == -1)
                return false;

            if (is_argument == 1)
                continue;
        }

        if (int result = 0 ; std::from_chars(args[i].data(), args[i].data() + args[i].size(), result).ec ==
            std::errc{}
            && int_args_
            .IsPositional()) {
            int_args_.SetParcedArgument(int_args_.GetPositional(), result);
        } else if (str_args_.IsPositional()) {
            str_args_.SetParcedArgument(str_args_.GetPositional(), args[i]);
        } else {
            bool is_any_wrong_key = false;

            for (size_t j = 1 ; j < args[i].size() - 1 ; ++j) {
                std::string arg = MergeChars(args[i][0], args[i][j]);

                if (flags_.KeyContains(arg)) {
                    arg = flags_.GetByKey(arg);
                }

                if (flags_.Contains(arg)) {
                    if (flags_.IsStored(arg)) {
                        *flags_.GetValue(arg) = true;
                    } else {
                        flags_.CreateValue(arg);
                    }
                } else {
                    is_any_wrong_key = true;
                    break;
                }
            }

            std::vector<std::string> cur_arg_config;
            cur_arg_config.push_back(MergeChars(args[i][0], args[i].back()));
            if (i + 1 < args.size())
                cur_arg_config.push_back(args[i + 1]);
            size_t delta = 0;

            if (this->IsArgument(cur_arg_config, delta) != 1)
                return false;

            i += delta;

            if (is_any_wrong_key) {
                PrintWarning("No such argument name, no any positional argument with same type:", args[i]);
                return false;
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
    return AddFlag(name, name, desc);
}

ArgParser& ArgParser::StoreValue(bool& value) {
    flags_.PutValue(cur_arg_, &value);
    return *this;
}

std::string ArgParser::HelpDescription() const {
    std::stringstream out;

    if (!is_added_help_) {
        out << "No help info provided";
        return out.str();
    }

    out << program_name_ << '\n';
    out << flags_.GetDescription("--help") << '\n';
    out << '\n';

    const auto& int_used = int_args_.GetUsedArgumentsList();
    const auto& string_used = str_args_.GetUsedArgumentsList();
    const auto& flag_used = flags_.GetUsedArgumentsList();



    const auto& int_keys = int_args_.GetKeyArgumentsList();
    const auto& string_keys = str_args_.GetKeyArgumentsList();
    const auto& flag_keys = flags_.GetKeyArgumentsList();

    for (auto& key_arg : int_keys) {
        // out << std::setw(4) << key_arg << ", " <<
    }

    return out.str();
}
ArgParser& ArgParser::Default(const int value) {
    int_args_.SetDefault(cur_arg_, value);
    return *this;
}
ArgParser& ArgParser::Default(const bool value) {
    flags_.SetDefault(cur_arg_, value);
    return *this;
}
ArgParser& ArgParser::Default(const char* value) {
    str_args_.SetDefault(cur_arg_, value);
    return *this;
}

bool ArgParser::IsArgumentCoincidence() const {
    std::vector<std::string_view> ins1, ins2, ins3;
    auto flags = flags_.GetUsedArgumentsList();
    auto ints = int_args_.GetUsedArgumentsList();
    auto strs = str_args_.GetUsedArgumentsList();
    std::ranges::set_intersection(flags, ints, std::back_inserter(ins1));
    std::ranges::set_intersection(flags, strs, std::back_inserter(ins2));
    std::ranges::set_intersection(ints, strs, std::back_inserter(ins3));
    return !ins1.empty() || !ins2.empty() || !ins3.empty();
}

int ArgParser::IsArgument(const std::vector<std::string>& args, size_t& i) {
    std::string arg = args[i].substr(0, args[i].find('='));
    std::string value = args[i].substr(args[i].find('=') + 1);

    if (flags_.KeyContains(arg)) {
        arg = flags_.GetByKey(arg);
    }

    if (flags_.Contains(arg)) {
        if (flags_.IsStored(arg)) {
            *flags_.GetValue(arg) = true;
        } else {
            flags_.CreateValue(arg);
        }
        return 1;
    }

    if (str_args_.Contains(arg) || str_args_.KeyContains(arg)) {
        // const auto stored = str_args_.GetValue(arg);
        if (arg == args[i]) {
            arg = str_args_.GetByKey(arg);
            ++i;
            if (i < args.size()) {
                value = args[i];
            } else if (!str_args_.IsDefault(arg)) {
                PrintWarning("Non-default argument missing value");

                return -1;
            }
        }

        str_args_.SetParcedArgument(arg, value);
        return 1;
    }
    if (int_args_.Contains(arg) || int_args_.KeyContains(arg)) {
        int res;
        if (arg == args[i]) {
            arg = int_args_.GetByKey(arg);
            ++i;
            if (i < args.size()) {
                auto [_, ec] = std::from_chars(args[i].data(), args[i].data() + args[i].size(), res);
                if (ec == std::errc::invalid_argument) {
                    PrintWarning("Given string instead int positional argument");

                    return -1;
                }
                if (ec == std::errc::result_out_of_range) {
                    PrintWarning("Given number more than an int");

                    return -1;
                }
            } else if (!int_args_.IsDefault(arg)) {
                PrintWarning("Non-default argument missing value");

                return -1;
            }
        } else {
            auto [_, ec] = std::from_chars(value.data(), value.data() + value.size(), res);
            if (ec == std::errc::invalid_argument) {
                PrintWarning("Not a number given as int argument");

                return -1;
            }
            if (ec == std::errc::result_out_of_range) {
                PrintWarning("Given number more than an int");

                return -1;
            }
        }
        int_args_.SetParcedArgument(arg, res);

        return 1;
    }

    return 0;
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
    if (int_args_.Contains(cur_arg_))
        int_args_.MakeMulti(cur_arg_);
    else if (str_args_.Contains(cur_arg_))
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
        // str_args_.MakeMulti(cur_arg_);
        str_args_.PutPositional(cur_arg_);
    } else if (IsIntArgument(int_args_, cur_arg_)) {
        // int_args_.MakeMulti(cur_arg_);
        int_args_.PutPositional(cur_arg_);
    }

    return *this;
}

std::string& ArgParser::GetStringValue(const char* name) {
    return *str_args_.GetValue(name);
}

int& ArgParser::GetIntValue(const std::string& name) {
    return *int_args_.GetValue(name);
}

bool& ArgParser::GetFlag(const std::string& name) {
    return *flags_.GetValue(name);
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

std::string BaseArgumentConfig::GetByKey(const std::string& key) const {
    if (!keys_.contains(key)) {
        PrintError("Can't find key argument", key);
    }
    return keys_.at(key);
}

std::string BaseArgumentConfig::GetDescription(const std::string& name) const {
    if (!desc_.contains(name))
        PrintError("Can't find description for arugment", name);
    return desc_.at(name);
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

const std::set<std::string>& BaseArgumentConfig::GetUsedArgumentsList() const {
    return used_;
}
bool BaseArgumentConfig::IsDefault(const std::string& arg) const {
    return is_default_.contains(arg);
}

std::vector<std::string> BaseArgumentConfig::GetKeyArgumentsList() const {
    std::vector<std::string> res;
    res.reserve(keys_.size());
    for (auto& [key, _] : keys_) {
        res.push_back(key);
    }
    return res;
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
    return names_.at(name);
}

void StringArgumentConfig::PutValues(const std::string& name,
                                     std::vector<std::string>* values) {
    multi_.insert({name, values});
}

void StringArgumentConfig::PutPositional(const std::string& arg) {
    if (!positional_.empty())
        PrintWarning("Positional argument redefined from", positional_);
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
    names_.insert({name, &cvalue_.at(name)});
}

void StringArgumentConfig::CreateValues(const std::string& name) {
    cvalues_.insert({name, {}});
    multi_.insert({name, &cvalues_.at(name)});
}

void StringArgumentConfig::AddValue(const std::string& arg, std::string value) {
    if (!multi_.contains(arg)) {
        PrintError("Can't set multi value argument:", arg);
    }
    cvalues_.at(arg).push_back(value);
}

bool StringArgumentConfig::IsStored(const std::string& arg) const {
    return names_.contains(arg) || multi_.contains(arg);
}

void StringArgumentConfig::SetDefault(const std::string& arg, const std::string& value) {
    this->CreateValue(arg, value);
    is_default_.insert(arg);
}

void StringArgumentConfig::SetParcedArgument(const std::string& arg, const std::string value) {
    if (this->IsMultiValueArgument(arg)) {
        if (this->IsStored(arg)) {
            this->GetValues(arg)->push_back(value);
        } else {
            if (!multi_.contains(arg)) {
                this->CreateValues(arg);
            }
            this->AddValue(arg, value);
        }
    } else {
        if (this->IsStored(arg)) {
            *this->GetValue(arg) = value;
        } else {
            this->CreateValue(arg, value);
        }
    }
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
    return multi_.at(name);
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
    return names_.at(name);
}
std::vector<int>*& IntArgumentConfig::GetValues(const std::string& name) {
    return multi_.at(name);
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
void IntArgumentConfig::CreateValues(const std::string& name) {
    cvalues_.insert({name, {}});
    multi_.insert({name, &cvalues_[name]});
}
void IntArgumentConfig::AddValue(const std::string& name, int value) {
    if (!cvalues_.contains(name)) {
        PrintError("Can't set multi value argument:", name);
    }
    cvalues_[name].push_back(value);
}

bool IntArgumentConfig::IsStored(const std::string& arg) const {
    return names_.contains(arg) || multi_.contains(arg);
}
void IntArgumentConfig::SetDefault(const std::string& arg, int value) {
    this->CreateValue(arg, value);
    is_default_.insert(arg);
}
void IntArgumentConfig::SetParcedArgument(const std::string& arg, const int value) {
    if (this->IsMultiValueArgument(arg)) {
        if (this->IsStored(arg)) {
            this->GetValues(arg)->push_back(value);
        } else {
            if (!multi_.contains(arg)) {
                this->CreateValues(arg);
            }
            this->AddValue(arg, value);
        }
    } else {
        if (this->IsStored(arg)) {
            *this->GetValue(arg) = value;
        } else {
            this->CreateValue(arg, value);
        }
    }
}

bool ArgParser::Help() const {
    return is_added_help_;
}

void FlagConfig::PutValue(const std::string& name, bool* value) {
    names_.insert({name, value});
}
void FlagConfig::MakeMulti(const std::string& basic_string) {
    PrintWarning("try to make multivalue flag argument");
}

bool*& FlagConfig::GetValue(const std::string& name) {
    if (!names_.contains(name)) {
        PrintError("No such argument in parser:", name);
        exit(EXIT_FAILURE);
    }
    return names_[name];
}
void FlagConfig::CreateValue(const std::string& arg) {
    cvalue_.insert({arg, true});
    names_.insert({arg, &cvalue_[arg]});
}
void FlagConfig::CreateValue(const std::string& arg, bool value) {
    cvalue_.insert({arg, value});
    names_.insert({arg, &cvalue_[arg]});
}
void FlagConfig::SetDefault(const std::string& arg, const bool value) {
    CreateValue(arg, value);
    is_default_.insert(arg);
}
bool FlagConfig::IsStored(const std::string& arg) {
    return names_.contains(arg);
}
} // namespace ArgumentParser
