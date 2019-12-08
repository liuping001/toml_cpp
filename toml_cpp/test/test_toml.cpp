//
// Created by lp on 2019/12/8.
//

#include "config_toml.hpp"
#include <memory>
#include <vector>
#include <iostream>

using namespace config_toml;
int main() {
  auto root = cpptoml::parse_file("toml_config.toml");
  Root r;
  r.FromToml(root);
  std::cout << r.title() <<std::endl;
  std::cout << r.database.connection_max.I() << std::endl;
  for (auto &item : r.clients.hosts) {
    std::cout << item() << std::endl;
  }
  std::cout << r.owner.dob.LocalDatetime() << std::endl;
}