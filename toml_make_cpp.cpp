//
// Created by lp on 2019/12/7.
//
#define CPPTOML_NO_RTTI
#include "cpptoml.h"
#include <iostream>
#include <memory>
#include <algorithm>

const char *kTomlBase = "\n#ifndef TOML_BASE_STRUCT\n"
                        "#define TOML_BASE_STRUCT\n"
                        "struct TomlBase {\n"
                        "  TomlBase(){}\n"
                        "  TomlBase(std::shared_ptr<cpptoml::base> ptr) : ptr_(ptr) {}\n"
                        "  std::string operator ()() const { return ptr_->as<std::string>()->get(); }\n"
                        "  int64_t I() const { return ptr_->as<int64_t>()->get(); }\n"
                        "  double D() const { return ptr_->as<double>()->get(); }\n"
                        "  bool B() const { return ptr_->as<bool>()->get(); }\n"
                        "#if defined(TOML_DATE)\n"
                        "  cpptoml::local_date LocalDate() const { return ptr_->as<cpptoml::local_date>()->get(); }\n"
                        "  cpptoml::local_time LocalTime() const { return ptr_->as<cpptoml::local_time>()->get(); }\n"
                        "  cpptoml::local_datetime LocalDatetime() const { return ptr_->as<cpptoml::local_datetime>()->get(); }\n"
                        "  cpptoml::offset_datetime OffsetDatetime() const { return ptr_->as<cpptoml::offset_datetime>()->get(); }\n"
                        "#endif\n"
                        " private:\n"
                        "  std::shared_ptr<cpptoml::base> ptr_;\n"
                        "};\n"
                        "#endif\n\n";

namespace toml_cpp {

static std::string Tab;
static std::string toml_base_dir;

struct CppOut {
  std::ostringstream make_struct;
};


std::string Type(std::shared_ptr<cpptoml::base> ptr) {
  using namespace cpptoml;
  switch (ptr->type()) {
    case base_type::STRING: return "std::string";
    case base_type::LOCAL_TIME: return "cpptoml::local_time";
    case base_type::LOCAL_DATE: return "cpptoml::local_date";
    case base_type::LOCAL_DATETIME: return "cpptoml::local_datetime";
    case base_type::OFFSET_DATETIME: return "cpptoml::offset_datetime";
    case base_type::INT: return "int64_t";
    case base_type::FLOAT: return "double";
    case base_type::BOOL: return "bool";
  }
  return "error";
}

inline std::string BigWord(const std::string &org_word) {
  auto to_up = [](char c) -> char {
    if (c >= 'a' && c <= 'z') {
      return 'A' + (c - 'a');
    }
    return c;
  };

  auto word = org_word;
  for (size_t i = 0; i < word.size(); i++) {
    if (i==0 || (i > 0 && word[i - 1] == '_')) {
      word[i] = to_up(word[i]);
    }
  }
  word.erase(std::remove_if(word.begin(), word.end(), [](char c) { return c == '_';}), word.end());
  return word;
}

void StructDefineArray(std::shared_ptr<cpptoml::base> ptr, CppOut &ss) {
  if (ptr->is_array()) {
    ss.make_struct << "std::vector<";
    StructDefineArray(*ptr->as_array()->begin(), ss);
    ss.make_struct << ">";
  } else {
    ss.make_struct << "TomlBase";
  }
}

void MakeStructDefine(std::shared_ptr<cpptoml::base> ptr,
                      const std::string &key,
                      CppOut &ss,
                      const std::string &depth) {
  auto next_depth = depth + Tab;
  if (ptr->is_array()) {
    ss.make_struct << depth;
    StructDefineArray(ptr, ss);
    ss.make_struct << " " << key << ";\n";
  } else if (ptr->is_table()) {
    ss.make_struct << depth << BigWord(key) << " " << key << "; \n";
  } else if (ptr->is_table_array()) {
    ss.make_struct << depth << "std::vector<" << BigWord(key) << "> " << key << ";\n";
  } else {
    ss.make_struct << depth << Type(ptr) <<" " << key << ";\n";
  }
}

void MakeInitArray(std::shared_ptr<cpptoml::base> ptr, std::ostringstream &init_func, const std::string &depth,
                   const std::string &key, int32_t d) {
  std::string item1 = "arr_" + key + "_item" + std::to_string(d - 1);
  std::string item2 = "arr_" + key + "_item" + std::to_string(d);

  auto n_depth = depth + Tab;
  auto nn_depth = n_depth + Tab;
  auto first_item = ptr->is_array() ? *ptr->as_array()->begin() : nullptr;
  if (first_item && first_item->is_array()) {
    init_func << n_depth << "for (auto item : *arr_" << key << "->as_array()) {\n";
    init_func << nn_depth << "auto arr_data = item;\n";
    init_func << nn_depth << "decltype(" << item1 << ")::value_type " << item2 << ";\n";
    MakeInitArray(first_item, init_func, n_depth, key, d + 1);
    init_func << nn_depth << item1 << ".push_back(" << item2 << ");\n";
    init_func << n_depth << "}\n";
  } else {
    if (ptr->is_table_array()) {
      init_func << n_depth << "for (auto item : *arr_" << key << "->as_table_array()) {\n";
      init_func << nn_depth << item1 << ".emplace_back();\n";
      init_func << nn_depth << item1 << ".back().FromToml(item);\n";
      init_func << n_depth << "}\n";
    } else {
      init_func << n_depth << "for (auto item : *arr_data->as_array()) {\n";
      init_func << nn_depth << item1 << ".push_back(item);\n";
      init_func << n_depth << "}\n";
    }
  }
}

void MakeInitFunc(std::shared_ptr<cpptoml::base> ptr,
                  const std::string &key,
                  std::ostringstream &init_func,
                  const std::string &depth) {
  auto n_depth = depth + Tab;
  if (ptr->is_array() || ptr->is_table_array()) {
    auto d = 1;
    init_func << n_depth << "auto arr_" << key << " = ptr->as_table()->get(\"" << key << "\");\n";
    init_func << n_depth << "decltype(" << key << ") arr_" << key << "_item" << d << ";\n";
    MakeInitArray(ptr, init_func, depth, key, d + 1);
    init_func << n_depth << key << " = " << "std::move(arr_" << key << "_item" << d << ");\n";
  } else if (ptr->is_table()) {
    init_func << n_depth << key << ".FromToml(ptr->as_table()->get(\"" << key << "\"));\n";
  } else {
    init_func << n_depth << key << " = ptr->as_table()->get(\"" << key << "\")->as<"<< Type(ptr) <<">()->get();\n";
  }
}

// 由于依赖关系，所以先遍历到最低层数据结构 然后才分析结构
void PrintStruct(std::shared_ptr<cpptoml::base> ptr, const std::string &key, CppOut &ss, const std::string &depth) {
  auto n_depth = depth + Tab;
  auto nn_depth = n_depth + Tab;
  std::vector<std::pair<std::string, std::shared_ptr<cpptoml::base>>> depend;
  if (ptr->is_table() || ptr->is_table_array()) {
    auto &type_table = ptr->is_table() ? *ptr->as_table() : **ptr->as_table_array()->begin();
    for (auto &item : type_table) {
      if (item.second->is_table() || item.second->is_table_array()) {
        depend.emplace_back(item.first, item.second);
      }
    }
  }

  for (auto item : depend) {
    PrintStruct(item.second, item.first, ss, depth);
  }

  std::ostringstream init_func;
  if (ptr->is_table() || ptr->is_table_array()) {
    auto &type_table = ptr->is_table() ? *ptr->as_table() : **ptr->as_table_array()->begin();
    ss.make_struct << "\n" << depth << "struct " << BigWord(key) << " {\n";
    init_func << "\n" << n_depth << "void FromToml(std::shared_ptr<cpptoml::base> ptr){\n";
    for (auto &item : type_table) {
      MakeStructDefine(item.second, item.first, ss, n_depth);
      MakeInitFunc(item.second, item.first, init_func, n_depth);
    }
    init_func << n_depth << "}\n";
    ss.make_struct << init_func.str();
    ss.make_struct << depth << "};\n";
  }
}

} // end namespace toml_cpp
const std::string basename(const std::string &path) {
  auto const pos = path.find_last_of('/');
  return pos == path.npos ? path : path.substr(pos + 1);
}

const std::string dirname(const std::string &path) {
  auto const pos = path.find_last_of('/');
  return pos == path.npos ? "" : path.substr(0, pos);
}


using namespace toml_cpp;
int main(int argc, char *argv[]) {
  std::string file;
  auto tab = 2;
  if (argc < 3) {
    std::cout << "toml_cpp -file file [-tab 2] [-date 1]\n";
    return -1;
  }
  for (int i = 2; i < argc; i = i + 2) {
    if (strcmp(argv[i - 1], "-file") == 0) {
      file = argv[i];
    } else if (strcmp(argv[i - 1], "-tab") == 0) {
      tab = std::stoi(argv[i]);
    } else if (strcmp(argv[i - 1], "-toml_base") == 0) {
      toml_base_dir = argv[i];
    } else {
      std::cout << "not find option: " << argv[i - 1] << std::endl;
      return -2;
    }
  }
  for (auto i = 0; i < tab; i++) {
    Tab.push_back(' ');
  }

  try {
    auto root = cpptoml::parse_file(file);
    auto file_name = std::string(basename(file));
    auto hpp = file_name.substr(0, file_name.find("."));

    CppOut out;
    PrintStruct(root, "root", out, "");

    std::ofstream fout(hpp + "_toml.hpp", std::ios::out);

    fout << "#pragma once\n";
    fout << "#include \"thirdparty/cpptoml/cpptoml.h\"\n\n";
    if (toml_base_dir=="1" || toml_base_dir=="true") {
      fout <<"#define TOML_DATE 1";
    }
    fout << kTomlBase;
    fout << "namespace " << hpp << "_toml {\n";
    fout << out.make_struct.str() << "\n";
    fout << "} // end " << hpp << "_toml\n";
    fout.close();
    std::cout << "finish toml cpp:" << file_name << std::endl;
  } catch (std::exception e) {
    std::cout << "parse file failed :" << argv[1] << " e:"<<e.what();
  }
  return 0;
}
