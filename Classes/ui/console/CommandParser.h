// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMMAND_PARSER_H_
#define VIGILANTE_COMMAND_PARSER_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace vigilante {

class CommandParser {
 public:
  CommandParser();
  virtual ~CommandParser() = default;

  void parse(const std::string& cmd, bool showNotification);

 private:
  void setSuccess();
  void setError(const std::string& errMsg);

  // Command handlers.
  void startQuest(const std::vector<std::string>& args);
  void addItem(const std::vector<std::string>& args);
  void removeItem(const std::vector<std::string>& args);

  bool _success;
  std::string _errMsg;
};

} // namespace vigilante

#endif // VIGILANTE_COMMAND_PARSER_H_
