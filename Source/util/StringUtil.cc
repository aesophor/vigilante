// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "StringUtil.h"

#include <sstream>

using namespace std;

namespace requiem::string_util {

vector<string> split(const string& s, const char delimiter) {
  stringstream ss{s};
  string t;
  vector<string> tokens;

  while (std::getline(ss, t, delimiter)) {
    if (t.length() > 0) {
      tokens.emplace_back(std::move(t));
    }
  }
  return tokens;
}

vector<string> parseArgs(const string& s) {
  stringstream ss{s};
  vector<string> tokens;
  string token;
  unsigned counter = 0;

  while (std::getline(ss, token, '\"')) {
    ++counter;
    if (counter % 2 == 0) {
      if (!token.empty()) {
        tokens.emplace_back(std::move(token));
      }
    } else {
      stringstream token_ss{token};
      while (std::getline(token_ss, token, ' ')) {
        if (!token.empty()) {
          tokens.emplace_back(std::move(token));
        }
      }
    }
  }
  return tokens;
}

bool startsWith(const string& s, const string& keyword) {
  return s.find(keyword) == 0;
}

bool contains(const string& s, const string& keyword) {
  return s.find(keyword) != string::npos;
}

void replace(string& s, const string& keyword, const string& newword) {
  string::size_type pos = s.find(keyword);

  while(pos != std::string::npos) {
    s.replace(pos, keyword.size(), newword);
    pos = s.find(keyword, pos + newword.size());
  }
}

void strip(string& s) {
  static const char* whitespace_chars = " \n\r\t";
  s.erase(0, s.find_first_not_of(whitespace_chars));
  s.erase(s.find_last_not_of(whitespace_chars) + 1);
}

string& toUpper(string& s) {
  for (auto& c : s) {
    if (c >= 'a' && c <= 'z') {
      c -= 'a' - 'A';
    }
  }
  return s;
}

string& toLower(string& s) {
  for (auto& c : s) {
    if (c >= 'A' && c <= 'Z') {
      c += 'a' - 'A';
    }
  }
  return s;
}

}  // namespace requiem::string_util
