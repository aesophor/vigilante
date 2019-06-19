#include "JsonUtil.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "cocos2d.h"

using std::string;
using std::vector;
using std::ifstream;
using std::stringstream;
using std::runtime_error;
using rapidjson::Document;

namespace vigilante {

namespace json_util {

Document parseJson(const string& jsonFileName) {
  ifstream fin(jsonFileName);
  if (!fin.is_open()) {
    throw std::runtime_error("Json not found: " + jsonFileName);
  }

  string content;
  string line;
  while (std::getline(fin, line)) {
    content += line;
  }
  fin.close();

  Document json;
  json.Parse(content.c_str());
  return json;
}

vector<string> splitString(const string& s, const char delimiter) {
	stringstream ss(s);
	string t;
	vector<string> tokens;

	while (std::getline(ss, t, delimiter)) {
		if (t.length() > 0) {
			tokens.push_back(t);
		}
	}
	return tokens;
}

} // namespace json_util

} // namespace vigilante
