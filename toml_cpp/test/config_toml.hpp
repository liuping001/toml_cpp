#pragma once
#include "toml_base.hpp"

namespace config_toml {

struct Clients {
    std::vector<TomlBase> hosts;
    std::vector<std::vector<TomlBase>> data;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        auto arr_hosts = ptr->as_table()->get("hosts");
        decltype(hosts) arr_hosts_item1;
        for (auto item : *arr_hosts->as_array()) {
            arr_hosts_item1.push_back(item);
        }
        hosts = std::move(arr_hosts_item1);
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

struct Database {
    TomlBase enabled;
    TomlBase connection_max;
    std::vector<TomlBase> ports;
    TomlBase server;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        enabled = ptr->as_table()->get("enabled");
        connection_max = ptr->as_table()->get("connection_max");
        auto arr_ports = ptr->as_table()->get("ports");
        decltype(ports) arr_ports_item1;
        for (auto item : *arr_ports->as_array()) {
            arr_ports_item1.push_back(item);
        }
        ports = std::move(arr_ports_item1);
        server = ptr->as_table()->get("server");
    }
};

struct Owner {
    TomlBase dob;
    TomlBase name;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        dob = ptr->as_table()->get("dob");
        name = ptr->as_table()->get("name");
    }
};

struct Beta {
    TomlBase dc;
    TomlBase ip;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        dc = ptr->as_table()->get("dc");
        ip = ptr->as_table()->get("ip");
    }
};

struct Alpha {
    TomlBase dc;
    TomlBase ip;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        dc = ptr->as_table()->get("dc");
        ip = ptr->as_table()->get("ip");
    }
};

struct Servers {
    Beta beta; 
    Alpha alpha; 

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        beta.FromToml(ptr->as_table()->get("beta"));
        alpha.FromToml(ptr->as_table()->get("alpha"));
    }
};

struct Root {
    Clients clients; 
    Database database; 
    Owner owner; 
    Servers servers; 
    TomlBase title;

    void FromToml(std::shared_ptr<cpptoml::base> ptr){
        clients.FromToml(ptr->as_table()->get("clients"));
        database.FromToml(ptr->as_table()->get("database"));
        owner.FromToml(ptr->as_table()->get("owner"));
        servers.FromToml(ptr->as_table()->get("servers"));
        title = ptr->as_table()->get("title");
    }
};

} // end config_toml
