#pragma once
#include "toml_base.hpp"

namespace config_toml {

struct Connect_info {
    TomlBase port;
    TomlBase ip;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        port = ptr->as_table()->get("port");
        ip = ptr->as_table()->get("ip");
    }
};

struct Servers {
    std::vector<Connect_info> connect_info;
    std::vector<std::vector<TomlBase>> data;
    TomlBase enabled;
    TomlBase connection_max;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        auto arr_connect_info = ptr->as_table()->get("connect_info");
        decltype(connect_info) arr_connect_info_item1;
        for (auto item : *arr_connect_info->as_table_array()) {
            arr_connect_info_item1.emplace_back();
            arr_connect_info_item1.back().FromToml(item);
        }
        connect_info = std::move(arr_connect_info_item1);
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
        enabled = ptr->as_table()->get("enabled");
        connection_max = ptr->as_table()->get("connection_max");
    }
};

struct Root {
    Servers servers; 
    TomlBase title;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        servers.FromToml(ptr->as_table()->get("servers"));
        title = ptr->as_table()->get("title");
    }
};

} // end config_toml
