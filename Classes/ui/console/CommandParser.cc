// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandParser.h"

#include <vector>

#include "character/Player.h"
#include "map/GameMapManager.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

using std::string;
using std::vector;

namespace vigilante {

void CommandParser::parse(const string& cmd) const {
  vector<string> tokens = string_util::split(cmd);
  
  if (tokens.front() == "startquest") {
    if (tokens.size() < 2) {
      VGLOG(LOG_ERR, "startquest: missing parameter `quest`");
      return;
    }
    Player* player = GameMapManager::getInstance()->getPlayer();
    player->getQuestBook().startQuest(tokens[1]);
  } else {
    VGLOG(LOG_ERR, "Unable to parse this line.");
  }
}

} // namespace vigilante
