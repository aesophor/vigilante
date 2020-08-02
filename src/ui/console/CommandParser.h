// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
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
  void updateDialogueTree(const std::vector<std::string>& args);
  void joinPlayerParty(const std::vector<std::string>& args);
  void leavePlayerParty(const std::vector<std::string>& args);
  void playerPartyMemberWait(const std::vector<std::string>& args);
  void playerPartyMemberFollow(const std::vector<std::string>& args);
  void tradeWithPlayer(const std::vector<std::string>& args);

  bool _success;
  std::string _errMsg;
};

}  // namespace vigilante

#endif  // VIGILANTE_COMMAND_PARSER_H_
