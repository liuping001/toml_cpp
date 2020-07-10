# toml_cpp
根据toml配置文件生成c++的struct，增强toml配置文件的读取
例如:  
```toml
title = "TOML Example"
[servers]
  connection_max = 5000
  enabled = true
  addr = ["127.0.0.1:3001", "127.0.0.1:3002"]

[clients]
  game = [ ["game1", "game2"], [1001, 1002] ]

[redis]
  [[redis.connect_info]]
    ip = "192.168.0.1"
    port = "9000"
  [[redis.connect_info]]
    ip = "192.168.0.2"
    port = "9001"

[mysql]
  # 你可以依照你的意愿缩进。使用空格或Tab。TOML不会在意。
  [mysql.record]
  ip = "10.0.0.1"
  pwd = "eqdc10"

  [mysql.conf]
  ip = "10.0.0.2"
  pwd = "eqdc10"

```

测试输出:   
```cpp
TEST_F(test_toml) {
  auto root = cpptoml::parse_file("config.toml");
  Root config;
  config.FromToml(root);
  ASSERT_EQ(config.title, "TOML Example");

  ASSERT_EQ(config.servers.connection_max, 5000);
  ASSERT_EQ(config.servers.enabled, true);
  ASSERT_EQ(config.servers.addr.size(), 2);
  ASSERT_EQ(config.servers.addr[0](), "127.0.0.1:3001");
  ASSERT_EQ(config.servers.addr[1](), "127.0.0.1:3002");

  ASSERT_EQ(config.clients.game.size(), 2);
  ASSERT_EQ(config.clients.game[0].size(), 2);
  ASSERT_EQ(config.clients.game[1].size(), 2);
  ASSERT_EQ(config.clients.game[0][0](), "game1");
  ASSERT_EQ(config.clients.game[0][1](), "game2");
  ASSERT_EQ(config.clients.game[1][0].I(), 1001);
  ASSERT_EQ(config.clients.game[1][1].I(), 1002);

  ASSERT_EQ(config.redis.connect_info.size(), 2);
  ASSERT_EQ(config.redis.connect_info[0].ip, "192.168.0.1");
  ASSERT_EQ(config.redis.connect_info[0].port, "9000");
  ASSERT_EQ(config.redis.connect_info[1].ip, "192.168.0.2");
  ASSERT_EQ(config.redis.connect_info[1].port, "9001");

  ASSERT_EQ(config.mysql.record.ip, "10.0.0.1");
  ASSERT_EQ(config.mysql.record.pwd, "eqdc10");
  ASSERT_EQ(config.mysql.conf.ip, "10.0.0.2");
  ASSERT_EQ(config.mysql.conf.pwd, "eqdc10");
}

```