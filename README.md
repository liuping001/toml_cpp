# toml_cpp
根据toml配置文件生成c++的struct，增强toml配置文件的读取
例如:  
```toml
title = "TOML Example"
[servers]
  connection_max = 5000
  enabled = true
  data = [ ["game1", "game2"], [4, 2] ]
  [[servers.connect_info]]
    ip = "10.0.0.1"
    port = "9000"
  [[servers.connect_info]]
    ip = "10.0.0.2"
    port = "9001"

```
c++代码:  
```cpp

namespace config_toml {

struct ConnectInfo {
  std::string port;
  std::string ip;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){ ... }
};

struct Servers {
  std::vector<std::vector<TomlBase>> data;
  int64_t connection_max;
  std::vector<ConnectInfo> connect_info;
  bool enabled;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){ ... }
};

struct Root {
  Servers servers; 
  std::string title;

  void FromToml(std::shared_ptr<cpptoml::base> ptr){ ... }
};
}

```

测试输出:   
```cpp
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

/*
TOML Example
5000
true
data list1: game1 game2
data list2: 4 2
ip, port[10.0.0.1 ,9000]
ip, port[10.0.0.2 ,9001]
*/  
```