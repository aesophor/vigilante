// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandHandler.h"

#include <memory>

#include "Audio.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "gameplay/DialogueTree.h"
#include "item/Item.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

#define DEFAULT_ERR_MSG "unable to parse this line"

using namespace std;

namespace vigilante {

using CmdTable = unordered_map<string, void (CommandHandler::*)(const vector<string>&)>;

bool CommandHandler::handle(const string& cmd, bool showNotification) {
  if (cmd.empty()) {
    return false;
  }

  const vector<string> args = string_util::parseArgs(cmd);
  if (args.empty()) {
    return false;
  }

  _success = false;
  _errMsg = DEFAULT_ERR_MSG;

  static const CmdTable cmdTable = {
    {"startQuest",              &CommandHandler::startQuest             },
    {"setStage",                &CommandHandler::setStage               },
    {"addItem",                 &CommandHandler::addItem                },
    {"removeItem",              &CommandHandler::removeItem             },
    {"updateDialogueTree",      &CommandHandler::updateDialogueTree     },
    {"joinPlayerParty",         &CommandHandler::joinPlayerParty        },
    {"leavePlayerParty",        &CommandHandler::leavePlayerParty       },
    {"playerPartyMemberWait",   &CommandHandler::playerPartyMemberWait  },
    {"playerPartyMemberFollow", &CommandHandler::playerPartyMemberFollow},
    {"tradeWithPlayer",         &CommandHandler::tradeWithPlayer        },
    {"killCurrentTarget",       &CommandHandler::killCurrentTarget      },
    {"interact",                &CommandHandler::interact               },
    {"narrate",                 &CommandHandler::narrate                },
    {"beginBossFight",          &CommandHandler::beginBossFight         },
  };

  // Execute the corresponding command handler from _cmdTable.
  // The obtained value from _cmdTable is a class member function pointer.
  CmdTable::const_iterator it = cmdTable.find(args[0]);
  if (it != cmdTable.end()) {
    VGLOG(LOG_INFO, "Executing cmd: [%s].", cmd.c_str());
    (this->*((*it).second))(args);
  }

  if (!_success) {
    _errMsg = args[0] + ": " + _errMsg;
    VGLOG(LOG_ERR, "%s", _errMsg.c_str());
  }

  if (showNotification) {
    auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
    notifications->show((_success) ? cmd : _errMsg);
  }

  return _success;
}

void CommandHandler::setSuccess() {
  _success = true;
}

void CommandHandler::setError(const string& errMsg) {
  _success = false;
  _errMsg = errMsg;
}

void CommandHandler::startQuest(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: startQuest <quest>");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getPlayer()->getQuestBook().startQuest(args[1])) {
    setError("failed to start quest.");
    return;
  }

  setSuccess();
}

void CommandHandler::setStage(const vector<string>& args) {
  if (args.size() < 3) {
    setError("usage: setStage <quest> <stage_idx>");
    return;
  }

  int stageIdx = 0;
  try {
    stageIdx = std::stoi(args[2]);
  } catch (const invalid_argument& ex) {
    setError("invalid argument `stage_idx`");
    return;
  } catch (const out_of_range& ex) {
    setError("`stage_idx` is too large");
    return;
  } catch (...) {
    setError("unknown error");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getPlayer()->getQuestBook().setStage(args[1], stageIdx)) {
    setError("failed to set the stage of quest.");
    return;
  }

  setSuccess();
}

void CommandHandler::addItem(const vector<string>& args) {
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
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getPlayer()->addItem(std::move(item), amount);
  setSuccess();
}

void CommandHandler::removeItem(const vector<string>& args) {
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
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getPlayer()->removeItem(item.get(), amount);
  setSuccess();
}

void CommandHandler::updateDialogueTree(const vector<string>& args) {
  if (args.size() < 3) {
    setError("usage: updateDialogueTree <npcJson> <dialogueTreeJson>");
    return;
  }

  // TODO: Maybe add some argument check here?

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  dialogueMgr->setLatestNpcDialogueTree(args[1], args[2]);
  setSuccess();
}

void CommandHandler::joinPlayerParty(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc is already in player's party.");
    return;
  }

  player->getParty()->recruit(targetNpc);
  setSuccess();
}

void CommandHandler::leavePlayerParty(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  player->getParty()->dismiss(targetNpc);
  setSuccess();
}

void CommandHandler::playerPartyMemberWait(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFileName)) {
    setError("This Npc is already waiting for player.");
    return;
  }

  player->getParty()->askMemberToWait(targetNpc);
  setSuccess();
}

void CommandHandler::playerPartyMemberFollow(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (!player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFileName)) {
    setError("This Npc is not waiting for player yet.");
    return;
  }

  player->getParty()->askMemberToFollow(targetNpc);
  setSuccess();
}

void CommandHandler::tradeWithPlayer(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->getNpcProfile().isTradable) {
    setError("This Npc is not tradable.");
    return;
  }

  targetNpc->beginTrade();
  setSuccess();
}

void CommandHandler::killCurrentTarget(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  targetNpc->receiveDamage(player, 999);
  setSuccess();
}

void CommandHandler::interact(const vector<string>&) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("No player.");
    return;
  }

  const list<Interactable*>& interactables = player->getInRangeInteractables();
  if (interactables.empty()) {
    setError("No nearby interactable objects.");
    return;
  }

  for (auto interactable : interactables) {
    player->interact(interactable);
  }
  setSuccess();
}

void CommandHandler::narrate(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: narrate <narratives...>");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  dialogueMgr->setTargetNpc(nullptr);
  for (size_t i = 1; i < args.size(); i++) {
    dialogueMgr->getSubtitles()->addSubtitle(args[i]);
  }

  dialogueMgr->getSubtitles()->beginSubtitles();
  setSuccess();
}

void CommandHandler::beginBossFight(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: beginBossFight <bossStageProfileJsonPath>");
    return;
  }

  rapidjson::Document json = json_util::loadFromFile(args[1]);
  const string targetJsonFileName = json["target"].GetString();
  const string bgm = json["bgm"].GetString();

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  shared_ptr<Character> targetCharacter;
  for (const auto& actor : gmMgr->getGameMap()->getDynamicActors()) {
    auto c = std::dynamic_pointer_cast<Character>(actor);
    if (!c || c->getCharacterProfile().jsonFileName != targetJsonFileName) {
      continue;
    }
    targetCharacter = std::move(c);
  }

  if (!targetCharacter) {
    setError(string_util::format("Failed to find [%s] in the current game map.", targetJsonFileName.c_str()));
    return;
  }
  Audio::the().playBgm(bgm);

  targetCharacter->addOnKilledCallback([this, args]() { endBossFight(args); });

  setSuccess();
}

void CommandHandler::endBossFight(const vector<string>& args) {
  if (args.size() < 2) {
    setError("usage: endBossFight <bossStageProfileJsonPath>");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Audio::the().playBgm(gmMgr->getGameMap()->getBgmFileName());

  setSuccess();
}

}  // namespace vigilante
