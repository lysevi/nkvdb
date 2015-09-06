#pragma once

namespace storage {
struct append_result {
  append_result() { writed = ignored = 0; }
  append_result(const append_result& other) 
  {
      this->writed = other.writed;
      this->ignored = other.ignored;
  }
  
  append_result operator+(const append_result &other) {
    append_result res;
    res.writed = writed + other.writed;
    res.ignored = ignored + other.ignored;
    return res;
  }

  void operator=(const append_result &other) {
    this->writed = other.writed;
    this->ignored = other.ignored;
  }
  size_t writed;
  size_t ignored;
};
}
