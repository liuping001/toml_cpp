/*********************************************************
*
* 文件自动生成. tool: https://github.com/liuping001/toml_cpp
*
**********************************************************/

#pragma once
#include "third/cpptoml.h"


#ifndef TOML_BASE_STRUCT
#define TOML_BASE_STRUCT
struct TomlBase {
  TomlBase(){}
  TomlBase(std::shared_ptr<cpptoml::base> ptr) : ptr_(ptr) {}
  std::string operator ()() const { return ptr_->as<std::string>()->get(); }
  int64_t I() const { return ptr_->as<int64_t>()->get(); }
  double D() const { return ptr_->as<double>()->get(); }
  bool B() const { return ptr_->as<bool>()->get(); }
#if defined(TOML_DATE)
  cpptoml::local_date LocalDate() const { return ptr_->as<cpptoml::local_date>()->get(); }
  cpptoml::local_time LocalTime() const { return ptr_->as<cpptoml::local_time>()->get(); }
  cpptoml::local_datetime LocalDatetime() const { return ptr_->as<cpptoml::local_datetime>()->get(); }
  cpptoml::offset_datetime OffsetDatetime() const { return ptr_->as<cpptoml::offset_datetime>()->get(); }
#endif
 private:
  std::shared_ptr<cpptoml::base> ptr_;
};
#endif

namespace config_toml {

struct Conf {
  std::string pwd;
  std::string ip;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    pwd = ptr->as_table()->get("pwd")->as<std::string>()->get();
    ip = ptr->as_table()->get("ip")->as<std::string>()->get();
  }
};

struct Record {
  std::string pwd;
  std::string ip;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    pwd = ptr->as_table()->get("pwd")->as<std::string>()->get();
    ip = ptr->as_table()->get("ip")->as<std::string>()->get();
  }
};

struct Mysql {
  Conf conf;
  Record record;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    conf.FromToml(ptr->as_table()->get("conf"));
    record.FromToml(ptr->as_table()->get("record"));
  }
};

struct ConnectInfo {
  std::string port;
  std::string ip;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    port = ptr->as_table()->get("port")->as<std::string>()->get();
    ip = ptr->as_table()->get("ip")->as<std::string>()->get();
  }
};

struct Redis {
  std::vector<ConnectInfo> connect_info;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    auto arr_connect_info = ptr->as_table()->get("connect_info");
    decltype(connect_info) arr_connect_info_item1;
    for (auto item : *arr_connect_info->as_table_array()) {
      arr_connect_info_item1.emplace_back();
      arr_connect_info_item1.back().FromToml(item);
    }
    connect_info = std::move(arr_connect_info_item1);
  }
};

struct Clients {
  std::vector<std::vector<TomlBase>> data;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    auto arr_data = ptr->as_table()->get("data");
    decltype(data) arr_data_item1;
    for (auto item : *arr_data->as_array()) {
      auto arr_data = item;
      decltype(arr_data_item1)::value_type arr_data_item2;
      for (auto item : *arr_data->as_array()) {
        arr_data_item2.push_back(item);
      }
      arr_data_item1.push_back(arr_data_item2);
    }
    data = std::move(arr_data_item1);
  }
};

struct Servers {
  std::vector<TomlBase> addr;
  bool enabled;
  int64_t connection_max;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    auto arr_addr = ptr->as_table()->get("addr");
    decltype(addr) arr_addr_item1;
    for (auto item : *arr_addr->as_array()) {
      arr_addr_item1.push_back(item);
    }
    addr = std::move(arr_addr_item1);
    enabled = ptr->as_table()->get("enabled")->as<bool>()->get();
    connection_max = ptr->as_table()->get("connection_max")->as<int64_t>()->get();
  }
};

struct Root {
  Mysql mysql;
  Redis redis;
  Clients clients;
  Servers servers;
  std::string title;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){
    mysql.FromToml(ptr->as_table()->get("mysql"));
    redis.FromToml(ptr->as_table()->get("redis"));
    clients.FromToml(ptr->as_table()->get("clients"));
    servers.FromToml(ptr->as_table()->get("servers"));
    title = ptr->as_table()->get("title")->as<std::string>()->get();
  }
};

} // end config_toml