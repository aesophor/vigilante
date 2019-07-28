// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandParser.h"

#include <vector>

#include "character/Player.h"
#include "map/GameMapManager.h"
#include "ui/notifications/Notifications.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

using std::string;
using std::vector;

namespace vigilante {

void CommandParser::parse(const string& cmd) const {
  vector<string> tokens = string_util::split(cmd);
  string errMsg = "Unable to parse this line";
  
  if (tokens.front() == "startquest") {
    if (tokens.size() < 2) {
      errMsg = "startquest: missing parameter `quest`";
      VGLOG(LOG_ERR, "%s", errMsg.c_str());
      Notifications::getInstance()->show(errMsg);
      return;
    }
    Player* player = GameMapManager::getInstance()->getPlayer();
    player->getQuestBook().startQuest(tokens[1]);
    Notifications::getInstance()->show(cmd);
  } else {
    VGLOG(LOG_ERR, "%s", errMsg.c_str());
    Notifications::getInstance()->show(errMsg);
  }
}

} // namespace vigilante
