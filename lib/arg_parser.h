#pragma once

#ifndef ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H
#define ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace ArgumentParser {

class BaseArgumentConfig {
 public:
  std::string_view GetByKey(char key);
  std::string_view GetDescription(std::string_view name);
  bool Contains(std::string_view);
  void Update(char key, const char* name, const char* desc = "");
  void MakeMulti();

 private:
 protected:
  std::map<char, std::string_view> keys_;
  std::map<std::string_view, std::string_view> desc_;
  std::set<std::string_view> used_;

  bool is_multi_ = false;
};

//template<class T> struct ArgMode

class IntArgumentConfig {
 public:
  void PutValue(std::string_view name, int value);
  void PutValues(std::string_view name, std::vector<int>* values);
  void PutPositional(std::string_view arg);
  std::string*& GetValue(std::string_view name);
  std::vector<std::string>*& GetValues(std::string_view name);
  [[nodiscard]] std::string_view GetPositional() const;

 private:
  std::map<std::string_view, std::string*> names_;
  std::map<std::string_view, std::vector<std::string>*> multi_;
  std::string_view positional_{};
};

class StringArgumentConfig : public BaseArgumentConfig {
 public:
  void PutValue(std::string_view name, std::string* value);
  void PutValues(std::string_view name, std::vector<std::string>* values);
  void PutPositional(std::string_view arg);
  std::string*& GetValue(std::string_view name);
  std::vector<std::string>*& GetValues(std::string_view name);
  [[nodiscard]] std::string_view GetPositional() const;

 private:
  std::map<std::string_view, std::string*> names_;
  std::map<std::string_view, std::vector<std::string>*> multi_;
  std::string_view positional_{};
};

struct FlagConfig {
  std::map<char, std::string_view> keys_; // [key, name]
  std::map<std::string_view, std::string_view> desc_; // [name, desc]
  std::map<std::string_view, bool*> names_; // [name, stored value]
};

class ArgParser {
 public:
  explicit ArgParser(const char* name);

  ~ArgParser();

  bool Parse(const std::vector<std::string>& args);

  bool Parse(int argc, char** argv);

  ArgParser& AddFlag(const char* name, const char* desc);

  ArgParser& AddFlag(char key, const char* name, const char* desc);

  ArgParser& AddStringArgument(char key, const char* name, const char* desc);

  ArgParser& AddStringArgument(const char* name, const char* desc);

  ArgParser& AddHelp(const char* desc);

  ArgParser& StoreValue(bool& value);

  ArgParser& StoreValue(std::string& value);

  ArgParser& MultiValue(uint min_count = 0);

  ArgParser& StoreValues(std::vector<std::string>& values);

  ArgParser& Positional();

  std::string HelpDescription();

 private:
  const char* program_name_;
  std::string_view cur_arg_{};

  bool is_added_help_ = false;

  FlagConfig flags_;
  IntArgumentConfig int_args_;
  StringArgumentConfig str_args_;
};
}

#endif //ARG_PARSER_PAWKORCHAGIN_ARG_PARSER_H
