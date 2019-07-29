// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandParser.h"

#include "character/Player.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "ui/notifications/Notifications.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

#define DEFAULT_ERR_MSG "Unable to parse this line"

using std::string;
using std::vector;

namespace vigilante {

CommandParser::CommandParser() : _success(), _errMsg() {}


void CommandParser::parse(const string& cmd) {
  const vector<string>& args = string_util::split(cmd);
  _success = false;
  _errMsg = DEFAULT_ERR_MSG;

  // Call the corresponding command handler.
  if (args.front() == "startquest") {
    startQuest(args);
  } else if (args.front() == "additem") {
    addItem(args);
  }

  if (!_success) {
    VGLOG(LOG_ERR, "%s", _errMsg.c_str());
  }
  Notifications::getInstance()->show((_success) ? cmd : _errMsg);
}

void CommandParser::setSuccess() {
  _success = true;
}

void CommandParser::setError(const string& errMsg) {
  _success = false;
  _errMsg = errMsg;
}



void CommandParser::startQuest(const vector<string>& args) {
  if (args.size() < 2) {
    setError("startquest: missing parameter `quest`");
    return;
  }

  GameMapManager::getInstance()->getPlayer()->getQuestBook().startQuest(args[1]);
  setSuccess();
}


void CommandParser::addItem(const vector<string>& args) {
  if (args.size() < 2) {
    setError("additem: missing parameter `itemName`");
    return;
  }

  int amount = 1;
  if (args.size() >= 3) {
    try {
      amount = std::stoi(args[2]);
    } catch (const std::invalid_argument& ex) {
      setError("additem: invalid argument `amount`");
      return;
    } catch (const std::out_of_range& ex) {
      setError("additem: `amount` is too large");
      return;
    } catch (...) {
      setError("additem: unknown error");
      return;
    }
  }

  GameMapManager::getInstance()->getPlayer()->addItem(Item::create(args[1]), amount);
  setSuccess();
}

} // namespace vigilante
