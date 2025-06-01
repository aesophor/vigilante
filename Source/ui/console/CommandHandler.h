// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_CONSOLE_COMMAND_HANDLER_H_
#define REQUIEM_UI_CONSOLE_COMMAND_HANDLER_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace requiem {

namespace cmd {

constexpr char kEcho[] = "echo";
constexpr char kSetBgmVolume[] = "setbgmvolume";
constexpr char kStartQuest[] = "startquest";
constexpr char kSetStage[] = "setstage";
constexpr char kAddItem[] = "additem";
constexpr char kRemoveItem[] = "removeitem";
constexpr char kAddGold[] = "addgold";
constexpr char kRemoveGold[] = "removegold";
constexpr char kUpdateDialogueTree[] = "updatedialoguetree";
constexpr char kJoinPlayerParty[] = "joinplayerparty";
constexpr char kLeavePlayerParty[] = "leaveplayerparty";
constexpr char kPartyMemberWait[] = "partymemberwait";
constexpr char kPartyMemberFollow[] = "partymemberfollow";
constexpr char kTrade[] = "trade";
constexpr char kKill[] = "kill";
constexpr char kInteract[] = "interact";
constexpr char kNarrate[] = "narrate";
constexpr char kRest[] = "rest";
constexpr char kRentRoomCheckIn[] = "rentroomcheckin";
constexpr char kRentRoomCheckOut[] = "rentroomcheckout";
constexpr char kBeginBossFight[] = "beginbossfight";
constexpr char kSetInGameTime[] = "setingametime";
constexpr char kMoveTo[] = "moveto";

}  // namespace cmd

class CommandHandler final {
 public:
  bool handle(const std::string& cmd, bool showNotification);

 private:
  void setSuccess();
  void setError(const std::string& errMsg);

  // Command handlers.
  void echo(const std::vector<std::string>& args);
  void setBgmVolume(const std::vector<std::string>& args);
  void startQuest(const std::vector<std::string>& args);
  void setStage(const std::vector<std::string>& args);
  void addItem(const std::vector<std::string>& args);
  void removeItem(const std::vector<std::string>& args);
  void addGold(const std::vector<std::string>& args);
  void removeGold(const std::vector<std::string>& args);
  void updateDialogueTree(const std::vector<std::string>& args);
  void joinPlayerParty(const std::vector<std::string>& args);
  void leavePlayerParty(const std::vector<std::string>& args);
  void partyMemberWait(const std::vector<std::string>& args);
  void partyMemberFollow(const std::vector<std::string>& args);
  void trade(const std::vector<std::string>& args);
  void kill(const std::vector<std::string>& args);
  void interact(const std::vector<std::string>& args);
  void narrate(const std::vector<std::string>& args);
  void rest(const std::vector<std::string>& args);
  void rentRoomCheckIn(const std::vector<std::string>& args);
  void rentRoomCheckOut(const std::vector<std::string>& args);
  void beginBossFight(const std::vector<std::string>& args);
  void endBossFight(const std::vector<std::string>& args);
  void setInGameTime(const std::vector<std::string>& args);
  void moveTo(const std::vector<std::string>& args);

  bool _success{};
  std::string _errMsg;
};

}  // namespace requiem

#endif  // REQUIEM_UI_CONSOLE_COMMAND_HANDLER_H_
