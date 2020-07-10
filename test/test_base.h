//
// Created by liuping on 2019/2/14.
//

#pragma once

#include <iostream>
#include <string>

using std::cout;
using std::endl;

extern const char warning[] = "warning: ";
extern const char error[]   = "error: ";
extern const char succ[]    = "succ: ";
extern const char info[]    = "info: ";

template <const char * text>
struct Log {
  Log (const char *f, int line) {
    os << f <<":"<< line << " "<< text;
  }
  Log () {
    os << text;
  }
  ~Log() {
  }
  template <class T>
  Log &operator << (const T &t) {
    os << t;
    return *this;
  }
  std::ostream &os = std::cout;
};

using Warn  = Log<warning>;
using Error = Log<error>;
using Succ  = Log<succ>;
using Info  = Log<info>;

#define WARN   Warn(__FILE__, __LINE__)
#define ERROR  Error(__FILE__, __LINE__)
#define SUCC   Succ(__FILE__, __LINE__)
#define INFO   Info(__FILE__, __LINE__)



// ---------------------- assert ---------------------------
struct Tester {
  const char * file_;
  int line_;
  bool ok_ = true;
  std::string opt;

  Tester(const char * file, int line) : file_(file), line_(line) {};
  ~Tester() {
    if (ok_) { return; }
    std::cout << file_ << ":" << line_ << " failed [" << opt << "] " << std::flush;
    exit(1);
  }
  template <class X> inline void ok(const X &x) { ok_ = x ? true : false; }
  template <class X, class Y> inline void equal_to     (const X & x, const Y & y) { ok_ = x == y; opt += "=="; }
  template <class X, class Y> inline void not_equal_to (const X & x, const Y & y) { ok_ = x != y; opt += "!="; }
  template <class X, class Y> inline void greater_equal(const X & x, const Y & y) { ok_ = x >= y; opt += ">="; }
  template <class X, class Y> inline void greater      (const X & x, const Y & y) { ok_ = x > y;  opt += ">"; }
  template <class X, class Y> inline void less_equal   (const X & x, const Y & y) { ok_ = x <= y; opt += "<="; }
  template <class X, class Y> inline void less         (const X & x, const Y & y) { ok_ = x < y;  opt += "<"; }

};

#define ASSERT(b)       Tester(__FILE__, __LINE__).ok(b)
#define ASSERT_EQ(a, b) Tester(__FILE__, __LINE__).equal_to(a, b) // ==
#define ASSERT_NE(a, b) Tester(__FILE__, __LINE__).not_equal_to(a, b) // !=
#define ASSERT_GE(a, b) Tester(__FILE__, __LINE__).greater_equal(a, b) // >=
#define ASSERT_GT(a, b) Tester(__FILE__, __LINE__).greater(a, b) // >
#define ASSERT_LE(a, b) Tester(__FILE__, __LINE__).less_equal(a, b) // <=
#define ASSERT_LT(a, b) Tester(__FILE__, __LINE__).less(a, b) // <


//----------------------- main -------------------------------------
#define TEST_FINSH int main() { for (const auto& cb : tests) {cb();} }

#include <functional>
#include <vector>
bool Insert(std::function<void()> t, std::vector<std::function<void()>> &tests, std::string func_name) {
  tests.push_back([func_name, t](){
    Info() << "begin test function [" <<func_name <<"]\n";
    t();
    Info() << "end test function [" <<func_name <<"]\n";
  });
  return true;
}
std::vector<std::function<void()>> tests;


#define TEST_F(func) struct Class_##func { \
    static void func ();\
};\
bool b_##func = Insert(Class_##func::func, tests, #func);\
void Class_##func::func()


#include <memory>
#define TEST(base, func) struct Derive##base##func : public base { \
    void func(); \
};\
bool b_##base##func = Insert(std::bind(&Derive##base##func::func, std::make_shared<Derive##base##func>()), tests, #func);\
void Derive##base##func::func()