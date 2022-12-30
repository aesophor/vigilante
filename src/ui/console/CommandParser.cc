// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandParser.h"

#include <memory>

#include "character/Player.h"
#include "character/Npc.h"
#include "gameplay/DialogueTree.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/notifications/Notifications.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

#define DEFAULT_ERR_MSG "unable to parse this line"

using namespace std;

using CmdTable = std::unordered_map<std::string,
      void (vigilante::CommandParser::*)(const std::vector<std::string>&)>;

namespace vigilante {

CommandParser::CommandParser() : _success(), _errMsg() {}

void CommandParser::parse(const string& cmd, bool showNotification) {
  vector<string> args;
  if (cmd.empty() || (args = string_util::split(cmd)).empty()) {
    return;
  }

  _success = false;
  _errMsg = DEFAULT_ERR_MSG;

  // Command handler table.
  static const CmdTable cmdTable = {
    {"startQuest",              &CommandParser::startQuest             },
    {"addItem",                 &CommandParser::addItem                },
    {"removeItem",              &CommandParser::removeItem             },
    {"updateDialogueTree",      &CommandParser::updateDialogueTree     },
    {"joinPlayerParty",         &CommandParser::joinPlayerParty        },
    {"leavePlayerParty",        &CommandParser::leavePlayerParty       },
    {"playerPartyMemberWait",   &CommandParser::playerPartyMemberWait  },
    {"playerPartyMemberFollow", &CommandParser::playerPartyMemberFollow},
    {"tradeWithPlayer",         &CommandParser::tradeWithPlayer        },
    {"killCurrentTarget",       &CommandParser::killCurrentTarget      },
  };
 
  // Execute the corresponding command handler from _cmdTable.
  // The obtained value from _cmdTable is a class member function pointer.
  CmdTable::const_iterator it = cmdTable.find(args[0]);
  if (it != cmdTable.end()) {
    (this->*((*it).second))(args);
  }

  if (!_success) {
    _errMsg = args[0] + ": " + _errMsg;
    VGLOG(LOG_ERR, "%s", _errMsg.c_str());
  }

  if (showNotification) {
    Notifications::getInstance()->show((_success) ? cmd : _errMsg);
  }
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
    setError("usage: startQuest <quest>");
    return;
  }

  GameMapManager::getInstance()->getPlayer()->getQuestBook().startQuest(args[1]);
  setSuccess();
}


void CommandParser::addItem(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: addItem <itemName> [amount]");
    return;
  }

  int amount = 1;
  if (args.size() >= 3) {
    try {
      amount = std::stoi(args[2]);
    } catch (const invalid_argument& ex) {
      setError("invalid argument `amount`");
      return;
    } catch (const out_of_range& ex) {
      setError("`amount` is too large");
      return;
    } catch (...) {
      setError("unknown error");
      return;
    }
  }

  if (amount <= 0) {
    setError("`amount` has to be at least 1");
    return;
  }

  unique_ptr<Item> item = Item::create(args[1]);
  GameMapManager::getInstance()->getPlayer()->addItem(std::move(item), amount);
  setSuccess();
}


void CommandParser::removeItem(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: removeItem <itemName> [amount]");
    return;
  }

  int amount = 1;
  if (args.size() >= 3) {
    try {
      amount = std::stoi(args[2]);
    } catch (const invalid_argument& ex) {
      setError("invalid argument `amount`");
      return;
    } catch (const out_of_range& ex) {
      setError("`amount` is too large");
      return;
    } catch (...) {
      setError("unknown error");
      return;
    }
  }

  if (amount <= 0) {
    setError("`amount` has to be at least 1");
    return;
  }

  unique_ptr<Item> item = Item::create(args[1]);
  GameMapManager::getInstance()->getPlayer()->removeItem(item.get(), amount);
  setSuccess();
}


void CommandParser::updateDialogueTree(const vector<string>& args) {
  if (args.size() < 3) {
    setError("usage: updateDialogueTree <npcJson> <dialogueTreeJson>");
    return;
  }

  // TODO: Maybe add some argument check here?
 
  DialogueTree::setLatestNpcDialogueTree(args[1], args[2]);
  setSuccess();
}


void CommandParser::joinPlayerParty(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (targetNpc->isInPlayerParty()) {
    setError("This Npc is already in player's party.");
    return;
  }

  player->getParty()->recruit(targetNpc);
  setSuccess();
}


void CommandParser::leavePlayerParty(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isInPlayerParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  player->getParty()->dismiss(targetNpc);
  setSuccess();
}


void CommandParser::playerPartyMemberWait(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isInPlayerParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (player->getParty()->hasWaitingMember(targetNpc->getCharacterProfile().jsonFileName)) {
    setError("This Npc is already waiting for player.");
    return;
  }

  player->getParty()->askMemberToWait(targetNpc);
  setSuccess();
}


void CommandParser::playerPartyMemberFollow(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isInPlayerParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (!player->getParty()->hasWaitingMember(targetNpc->getCharacterProfile().jsonFileName)) {
    setError("This Npc is not waiting for player yet.");
    return;
  }

  player->getParty()->askMemberToFollow(targetNpc);
  setSuccess();
}


void CommandParser::tradeWithPlayer(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->getNpcProfile().isTradable) {
    setError("This Npc is not tradable.");
    return;
  }

  targetNpc->beginTrade();
  setSuccess();
}


void CommandParser::killCurrentTarget(const vector<string>&) {
  Player* player = GameMapManager::getInstance()->getPlayer();
  Npc* targetNpc = DialogueManager::getInstance()->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  targetNpc->receiveDamage(player, 999);
  setSuccess();
}

}  // namespace vigilante
