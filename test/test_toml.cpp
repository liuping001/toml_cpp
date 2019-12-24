//
// Created by lp on 2019/12/8.
//

#include "config_toml.hpp"
#include <memory>
#include <vector>
#include <iostream>

using namespace config_toml;
int main() {
  auto root = cpptoml::parse_file("config.toml");
  Root config;
  config.FromToml(root);
  std::cout << config.title <<std::endl;
  std::cout << config.servers.connection_max << std::endl;
  std::cout << (config.servers.enabled ? "true" : "false") << std::endl;
  std::cout << "data list1: ";
  for (auto &item1 : config.servers.data[0]) {
    std::cout << item1()<<" ";
  }
  std::cout << std::endl;
  std::cout << "data list2: ";
  for (auto &item2 : config.servers.data[1]) {
    std::cout << item2.I() <<" ";
  }
  std::cout << std::endl;
  for (auto &item : config.servers.connect_info) {
    std::cout << "ip, port[" << item.ip <<" ," << item.port <<"]" << std::endl;
  }
}