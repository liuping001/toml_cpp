//
// Created by lp on 2019/12/7.
//

#include "cpptoml.h"
#include <iostream>
#include <memory>

struct CppOut {
  std::ostringstream make_struct;
  std::ostringstream toml_cpp;
};

inline std::string BigWord(const std::string &org_word) {
  auto word = org_word;
  if (word[0] >= 'a' && word[0] <= 'z') {
    word[0] = 'A' + (word[0] - 'a');
  }
  return word;
}

void PrintArray(std::shared_ptr<cpptoml::base> ptr, CppOut &ss) {
  if (ptr->is_array()) {
    ss.make_struct << "std::vector<";
    PrintArray(*ptr->as_array()->begin(), ss);
    ss.make_struct << ">";
  } else {
    ss.make_struct << "TomlBase";
  }
}


void Print(std::shared_ptr<cpptoml::base> ptr, const std::string &key, CppOut &ss, std::string depth) {
  auto next_depth = depth + "    ";
  if (ptr->is_array()) {
    ss.make_struct << depth;
    PrintArray(ptr, ss);
    ss.make_struct << " " << key << ";\n";
  } else if (ptr->is_table()) {
    ss.make_struct << depth<< BigWord(key) <<" "<< key <<"; \n";
  } else if (ptr->is_table_array()) {
    ss.make_struct << depth << "std::vector<" << BigWord(key) <<"> " << key << ";\n";
  } else {
    ss.make_struct <<depth << "TomlBase "<< key <<";\n";
  }
}


void InitPrintArray(std::shared_ptr<cpptoml::base> ptr, std::ostringstream &init_func,std::string depth,
    const std::string &key, int32_t d) {
  std::string item1 = "arr_" + key + "_item" + std::to_string(d -1);
  std::string item2 = "arr_" + key + "_item" + std::to_string(d);

  auto n_depth = depth + "    ";
  auto nn_depth = n_depth + "    ";
  auto first_item = ptr->is_array() ? *ptr->as_array()->begin() : nullptr;
  if (first_item && first_item->is_array()) {
    init_func << n_depth<< "for (auto item : *arr_"<< key<< "->as_array()) {\n";
    init_func << nn_depth << "auto arr_data = item;\n";
    init_func << nn_depth <<  "decltype("<<item1 <<")::value_type "<< item2<< ";\n";
    InitPrintArray(first_item, init_func, n_depth, key, d + 1);
    init_func << nn_depth << item1 <<".push_back(" << item2 <<");\n";
    init_func <<n_depth<< "}\n";
  } else {
    if (ptr->is_table_array()) {
      init_func << n_depth << "for (auto item : *arr_" << key << "->as_table_array()) {\n";
      init_func << nn_depth << item1 << ".emplace_back();\n";
      init_func << nn_depth << item1 << ".back().FromToml(item);\n";
      init_func << n_depth << "}\n";
    } else {
      init_func << n_depth << "for (auto item : *arr_" << key << "->as_array()) {\n";
      init_func << nn_depth << item1 << ".push_back(item);\n";
      init_func << n_depth << "}\n";
    }
  }
}

void InitPrint(std::shared_ptr<cpptoml::base> ptr, const std::string &key, std::ostringstream &init_func, std::string depth) {
  auto n_depth = depth + "    ";
  if (ptr->is_array() || ptr->is_table_array()) {
    auto d = 1;
    init_func << n_depth << "auto arr_" << key<<" = ptr->as_table()->get(\"" << key << "\");\n";
    init_func << n_depth << "decltype("<<key<<") arr_"<<key<<"_item"<< d <<";\n";
    InitPrintArray(ptr, init_func, depth, key, d + 1);
    init_func << n_depth << key <<" = " <<  "std::move(arr_"<<key<<"_item"<< d <<");\n";
  }else if (ptr->is_table()) {
    init_func << n_depth << key <<".FromToml(ptr->as_table()->get(\""<< key<<"\"));\n";
  } else {
    init_func << n_depth << key << " = ptr->as_table()->get(\""<<key<<"\");\n";
  }
}

void PrintStruct(std::shared_ptr<cpptoml::base> ptr, const std::string &key, CppOut &ss, std::string depth) {
  auto n_depth = depth + "    ";
  auto nn_depth = n_depth + "    ";
  std::vector<std::pair<std::string, std::shared_ptr<cpptoml::base>>> depend;
  if (ptr->is_table()) {
    for (auto &item : *ptr->as_table()) {
      if (item.second->is_table() || item.second->is_table_array()) {
        depend.emplace_back(item.first, item.second);
      }
    }
  } else if (ptr->is_table_array()) {
    for (auto &item : **ptr->as_table_array()->begin()) {
      if (item.second->is_table() || item.second->is_table_array()) {
        depend.emplace_back(item.first, item.second);
      }
    }
  }

  for(auto item : depend) {
    PrintStruct(item.second, item.first, ss, depth);
  }

  std::ostringstream init_func;
  if (ptr->is_table()) {
    ss.make_struct << "\n" << depth << "struct " << BigWord(key) << " {\n";
    init_func <<"\n" << n_depth << "void FromToml(std::shared_ptr<cpptoml::base> ptr){\n";
    for (auto &item : *ptr->as_table()) {
      Print(item.second, item.first, ss, n_depth);
      InitPrint(item.second, item.first, init_func, n_depth);
    }
    init_func << n_depth <<"}\n";
    ss.make_struct << init_func.str();
    ss.make_struct << depth << "};\n";

  } else if (ptr->is_table_array()) {
    ss.make_struct << "\n" << depth << "struct " << BigWord(key) << " {\n";
    init_func <<"\n" << n_depth << "void FromToml(std::shared_ptr<cpptoml::base> ptr){\n";
    for (auto &item : **ptr->as_table_array()->begin()) {
      Print(item.second, item.first, ss, n_depth);
      InitPrint(item.second, item.first, init_func, n_depth);
    }
    init_func << n_depth <<"}\n";
    ss.make_struct << init_func.str();
    ss.make_struct << depth << "};\n";
  }
}

int main(int argc, char *argv[]) {
  auto root = cpptoml::parse_file(argv[1]);
  auto file_name = std::string(basename(argv[1]));
  auto hpp = file_name.substr(0, file_name.find("."));

  CppOut out;
  PrintStruct(root, "root", out, "");

  std::ofstream fout(hpp + "_toml.hpp", std::ios::out);
  fout << "#pragma once\n";
  fout << "#include \"toml_base.hpp\"\n\n";
  fout << "namespace " <<hpp <<"_toml {\n";
  fout << out.make_struct.str()<<"\n";
  fout << "} // end "<< hpp <<"_toml\n";
  fout.close();
  std::cout << "finish toml cpp:" << file_name << std::endl;
}
