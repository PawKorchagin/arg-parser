#include <sstream>

#include "arg_parser.h"

namespace {
void PrintError(const char* msg) {
    std::cerr << "Error: " << msg << '\n';
}
}

namespace ArgumentParser {
ArgParser::ArgParser(const char* name) : program_name_(name) {}


bool IsFlag(FlagConfig& flags_, std::string_view cur, std::string& name) {
    if (flags_.names_.contains(cur.substr(2))) {
        name = cur.substr(2);
    }

    if (flags_.keys_.contains(arg[1])) {
        name = flags_.keys_[arg[1]];
    }

    if (flags_.names_.contains(name)) {
        *flags_.names_[name] = true;
    }
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    if (args.size() < 2) {
//        PrintError("No arguments in program run configuration");
        return true;
    }

    for (size_t i = 1 ; i < args.size() ; ++i) {
        auto& arg = args[i];
        if (arg == "--help" || arg == "-h") {
            std::cerr << HelpDescription();
            return true;
        }

        std::string_view cur = arg;
        std::string name;

        bool proposal = true;

        if (IsFlag(flags_, arg, name)) {

        }

        if (str_args_.Contains(cur.substr(2, cur.find('=') - 2))) {
            name = cur.substr(2, cur.find('=') - 2);
        }

        if (!str_args_.GetByKey(arg[1]).empty()) {
            name = str_args_.GetByKey(arg[1]);
        }

        if (str_args_.Contains(name)) {
            auto stored = str_args_.GetValue(name);
            if (arg.size() <= 2) {
                ++i;
                if (i < args.size()) {
                    *stored = args[i];
                }
            } else {
                *stored = cur.substr(cur.find('=') + 1);
            }
            continue;
        }

        if (!str_args_.GetPositional().empty()) {
            auto stored = str_args_.GetValues(str_args_.GetPositional());
            stored->push_back(arg);
        }
    }

    return true;
}

bool ArgParser::Parse(int argc, char** argv) {
    return Parse({argv, argv + argc});
}

ArgParser& ArgParser::AddHelp(const char* desc) {
    is_added_help_ = true;
    return AddFlag('h', "help", desc);
}

ArgParser& ArgParser::AddFlag(char key, const char* name, const char* desc) {
    AddFlag(name, desc);
    flags_.keys_.insert({key, name});
    return *this;
}

ArgParser& ArgParser::AddFlag(const char* name, const char* desc) {
    cur_arg_ = name;
//    flags_.names_.insert({name, nullptr});
    flags_.desc_.insert({name, desc});
    return *this;
}

ArgParser& ArgParser::StoreValue(bool& value) {
    auto* ptr = &value;
    flags_.names_[cur_arg_] = ptr;
    return *this;
}

std::string ArgParser::HelpDescription() const {
    std::stringstream out;

    if (!is_added_help_) {
        out << "No info had provided yet";
    }

    return out.str();
}

ArgParser& ArgParser::AddStringArgument(const char* name, const char* desc) {
    return AddStringArgument('\0', name, desc);
}

ArgParser& ArgParser::AddStringArgument(char key, const char* name, const char* desc) {
    cur_arg_ = {name};
    str_args_.Update(key, name, desc);
    return *this;
}

ArgParser& ArgParser::StoreValue(std::string& value) {
    str_args_.PutValue(cur_arg_, &value);
    return *this;
}
ArgParser& ArgParser::MultiValue(uint min_count) {
    str_args_.MakeMulti();
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<std::string>& values) {
    str_args_.PutValues(cur_arg_, &values);
    return *this;
}
ArgParser& ArgParser::Positional() {
    std::cerr << cur_arg_ << '\n';
    if (str_args_.Contains(cur_arg_)) {
        str_args_.PutPositional(cur_arg_);
    }
    return *this;
}

ArgParser& ArgParser::AddIntArgument(char key, const char* name, const char* desc) {
    cur_arg_ = name;
    int_args_.Update(key, name, desc);
    return *this;
}

ArgParser& ArgParser::AddIntArgument(const char* name, const char* desc) {
    return AddIntArgument('\0', name, desc);
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

std::string_view BaseArgumentConfig::GetByKey(char key) {
    return keys_[key];
}

std::string_view BaseArgumentConfig::GetDescription(std::string_view name) {
    return desc_[name];
}

void BaseArgumentConfig::Update(char key, const char* name, const char* desc) {
    used_.insert(name);
    keys_.insert({key, name});
    desc_.insert({name, desc});
}

void BaseArgumentConfig::MakeMulti() {
    is_multi_ = true;
}

void StringArgumentConfig::PutValue(std::string_view name, std::string* value) {
    names_.insert({name, value});
}

bool BaseArgumentConfig::Contains(std::string_view name) {
    return used_.contains(name);
}

std::string*& StringArgumentConfig::GetValue(std::string_view name) {
    return names_[name];
}

void StringArgumentConfig::PutValues(std::string_view name, std::vector<std::string>* values) {
    multi_.insert({name, values});
}

void StringArgumentConfig::PutPositional(std::string_view arg) {
    positional_ = arg;
}

std::string_view StringArgumentConfig::GetPositional() const {
    return positional_;
}

std::vector<std::string>*& StringArgumentConfig::GetValues(std::string_view name) {
    return multi_[name];
}

void IntArgumentConfig::PutValue(std::string_view name, int* value) {
    names_.insert({name, value});
}

void IntArgumentConfig::PutValues(std::string_view name, std::vector<int>* values) {
    multi_.insert({name, values});
}
void IntArgumentConfig::PutPositional(std::string_view arg) {
    positional_ = arg;
}

int*& IntArgumentConfig::GetValue(std::string_view name) {
    return names_[name];
}
std::vector<int>*& IntArgumentConfig::GetValues(std::string_view name) {
    return multi_[name];
}
std::string_view IntArgumentConfig::GetPositional() const {
    return positional_;
}

bool ArgParser::Help() const {
    return is_added_help_;
}
}
