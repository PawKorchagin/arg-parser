#pragma once

#ifndef ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H
#define ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

template<class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T> vec) {
    out << "{";
    for (int i = 0 ; i < vec.size() - 1 ; ++i) {
        out << vec[i] << " ";
    }
    if (!vec.empty())
        out << vec.back();
    out << "}";

    return out;
}

namespace ArgumentParser {
enum class ArgumentCheckStatus {
    kCorrectArgument, kParsingFailure, kIncorrectArgument
};

class BaseArgumentConfig {
    public:
        virtual ~BaseArgumentConfig() = default;
        [[nodiscard]] std::string GetByKey(const std::string&) const;
        [[nodiscard]] std::string GetDescription(const std::string&) const;
        [[nodiscard]] bool KeyContains(const std::string&) const;
        [[nodiscard]] bool Contains(const std::string&) const;
        void SetArgument(const std::string&, const std::string&, const std::string& desc = "");
        virtual void MakeMulti(const std::string&);
        [[nodiscard]] bool IsMultiValueArgument(const std::string&) const;
        [[nodiscard]] std::vector<std::string> GetUsedArgumentsList() const;
        [[nodiscard]] bool IsDefault(const std::string&) const;
        [[nodiscard]] std::vector<std::string> GetKeyArgumentsList() const;
        std::string GetArgumentHelpDescription(const char*, const std::string&) const;

    private:
        struct ArgumentMainData {
            std::string key_;
            std::string desc_;
        };

    protected:
        // template<class T>
        // std::map<std::string, std::vector<T>> cvalues_;
        std::map<std::string, std::string> keys_;
        std::map<std::string, ArgumentMainData> used_;
        std::set<std::string> is_default_;
        std::set<std::string> is_multi_;
};

class IntArgumentConfig final : public BaseArgumentConfig {
    public:
        void PutValue(const std::string& name, int* value);
        void PutValues(const std::string& name, std::vector<int>* values);
        void PutPositional(const std::string& arg);
        int*& GetValue(const std::string&);
        std::vector<int>*& GetValues(const std::string& name);
        [[nodiscard]] std::string GetPositional() const;
        [[nodiscard]] bool IsPositional() const;
        void CreateValue(const std::string&, int);
        void CreateValues(const std::string&);
        void AddValue(const std::string&, int);
        [[nodiscard]] bool IsStored(const std::string&) const;
        void SetDefault(const std::string&, int);
        void SetParcedArgument(const std::string&, int);
        [[nodiscard]] std::string GetExtraArgumentsDescription(const std::string&) const;

    private:
        std::map<std::string, int*> names_;
        std::map<std::string, std::vector<int>*> multi_;
        std::map<std::string, int> cvalue_;
        std::map<std::string, std::vector<int> > cvalues_;
        std::string positional_{};
        bool is_positional_ = false;
};

class StringArgumentConfig final : public BaseArgumentConfig {
    public:
        void PutValue(const std::string& name, std::string* value);
        void PutValues(const std::string& name, std::vector<std::string>* values);
        void PutPositional(const std::string& arg);
        std::string*& GetValue(const std::string& name);
        std::vector<std::string>*& GetValues(const std::string& name);
        [[nodiscard]] std::string GetPositional() const;
        [[nodiscard]] bool IsPositional() const;
        void CreateValue(const std::string&, const std::string&);
        void CreateValues(const std::string&);
        void AddValue(const std::string&, const std::string&);
        [[nodiscard]] bool IsStored(const std::string&) const;
        void SetDefault(const std::string&, const std::string&);
        void SetParcedArgument(const std::string&, const std::string&);
        [[nodiscard]] std::string GetExtraArgumentsDescription(const std::string&) const;
        // void CreateValues(std::string, const std::string&);
        // void CreateValues(std::string, )
        // [[nodiscard]] bool IsSingleArgument(std::string) const;

    private:
        std::map<std::string, std::string*> names_;
        std::map<std::string, std::vector<std::string>*> multi_;
        std::map<std::string, std::string> cvalue_;
        std::map<std::string, std::vector<std::string> > cvalues_;
        std::string positional_{};
        bool is_positional = false;
};

class FlagConfig final : public BaseArgumentConfig {
    public:
        void PutValue(const std::string&, bool*);
        void MakeMulti(const std::string&) override;
        bool*& GetValue(const std::string&);
        void CreateValue(const std::string&);
        void CreateValue(const std::string&, bool);
        void SetDefault(const std::string&, bool);
        [[nodiscard]] bool IsStored(const std::string&) const;
        [[nodiscard]] std::string GetExtraArgumentsDescription(const std::string&) const;

    private:
        std::map<std::string, bool*> names_; // [name, stored value]
        std::map<std::string, bool> cvalue_;
};

class ArgParser {
    public:
        explicit ArgParser(std::string name);

        ~ArgParser();

        bool Parse(const std::vector<std::string>& args);

        bool Parse(int argc, char** argv);

        ArgParser& AddFlag(const std::string& name, const std::string& desc = "");

        ArgParser& AddFlag(const std::string&,
                           const std::string&,
                           const std::string& desc);

        ArgParser& AddStringArgument(const std::string&,
                                     const std::string&,
                                     const std::string& desc);

        ArgParser& AddStringArgument(const std::string&,
                                     const std::string& desc = "");

        ArgParser& AddIntArgument(const std::string&,
                                  const std::string&,
                                  const std::string& desc);

        ArgParser& AddIntArgument(const std::string&, const std::string& = "");

        ArgParser& AddHelp(const std::string&);

        ArgParser& StoreValue(bool&);

        ArgParser& StoreValue(int&);

        ArgParser& StoreValue(std::string&);

        ArgParser& MultiValue(uint min_count = 0);

        ArgParser& StoreValues(std::vector<std::string>&);

        ArgParser& StoreValues(std::vector<int>&);

        ArgParser& Positional();

        std::string& GetStringValue(const char*);

        int& GetIntValue(const std::string&);

        bool& GetFlag(const std::string&);

        [[nodiscard]] bool Help() const;

        [[nodiscard]] std::string HelpDescription() const;

        ArgParser& Default(int);

        ArgParser& Default(bool);

        ArgParser& Default(const char*);

    private:
        [[nodiscard]] bool IsArgumentCoincidence() const;
        ArgumentCheckStatus IsArgument(const std::vector<std::string>&, size_t&);
        [[nodiscard]] bool IsUnusedNoDefaultArgument() const;

        std::string program_name_;
        std::string cur_arg_{};

        bool is_added_help_ = false;

        FlagConfig flags_;
        IntArgumentConfig int_args_;
        StringArgumentConfig str_args_;
};
} // namespace ArgumentParser

#endif // ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H
