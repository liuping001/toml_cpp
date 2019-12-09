
#pragma once

#include <memory>
#include "cpptoml.h"

struct TomlBase {
  TomlBase(){}
  TomlBase(std::shared_ptr<cpptoml::base> ptr) : ptr_(ptr) {}
  std::string operator () () { return ptr_->as<std::string>()->get(); }
  int64_t I() { return ptr_->as<int64_t>()->get(); }
  double D() { return ptr_->as<double>()->get(); }
  bool B() { return ptr_->as<bool>()->get(); }
  cpptoml::local_date LocalDate() { return ptr_->as<cpptoml::local_date>()->get(); }
  cpptoml::local_time LocalTime() { return ptr_->as<cpptoml::local_time>()->get(); }
  cpptoml::local_datetime LocalDatetime() { return ptr_->as<cpptoml::local_datetime>()->get(); }
  cpptoml::offset_datetime OffsetDatetime() { return ptr_->as<cpptoml::offset_datetime>()->get(); }
 private:
  std::shared_ptr<cpptoml::base> ptr_;
};