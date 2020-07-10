//
// Created by lp on 2019/12/8.
//

#include "config_toml.hpp"
#include "test_base.h"

#include <memory>
#include <vector>
#include <iostream>

using namespace config_toml;

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

  ASSERT_EQ(config.clients.data.size(), 2);
  ASSERT_EQ(config.clients.data[0].size(), 2);
  ASSERT_EQ(config.clients.data[1].size(), 2);
  ASSERT_EQ(config.clients.data[0][0](), "game1");
  ASSERT_EQ(config.clients.data[0][1](), "game2");
  ASSERT_EQ(config.clients.data[1][0].I(), 1001);
  ASSERT_EQ(config.clients.data[1][1].I(), 1002);

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

TEST_FINSH