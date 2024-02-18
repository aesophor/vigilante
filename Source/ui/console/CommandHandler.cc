// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "CommandHandler.h"

#include <memory>

#include "character/Player.h"
#include "character/Npc.h"
#include "gameplay/DialogueTree.h"
#include "item/Item.h"
#include "item/Key.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

namespace {

using CmdTable = unordered_map<string, void (CommandHandler::*)(const vector<string>&)>;

constexpr char kDefaultErrMsg[] = "Unable to parse this line";

}  // namespace

bool CommandHandler::handle(const string& cmd, bool showNotification) {
  if (cmd.empty()) {
    return false;
  }

  const vector<string> args = string_util::parseArgs(cmd);
  if (args.empty()) {
    return false;
  }

  _success = false;
  _errMsg = kDefaultErrMsg;

  static const CmdTable cmdTable = {
    {cmd::kStartQuest,         &CommandHandler::startQuest         },
    {cmd::kSetStage,           &CommandHandler::setStage           },
    {cmd::kAddItem,            &CommandHandler::addItem            },
    {cmd::kRemoveItem,         &CommandHandler::removeItem         },
    {cmd::kAddGold,            &CommandHandler::addGold            },
    {cmd::kRemoveGold,         &CommandHandler::removeGold         },
    {cmd::kUpdateDialogueTree, &CommandHandler::updateDialogueTree },
    {cmd::kJoinPlayerParty,    &CommandHandler::joinPlayerParty    },
    {cmd::kLeavePlayerParty,   &CommandHandler::leavePlayerParty   },
    {cmd::kPartyMemberWait,    &CommandHandler::partyMemberWait    },
    {cmd::kPartyMemberFollow,  &CommandHandler::partyMemberFollow  },
    {cmd::kTrade,              &CommandHandler::trade              },
    {cmd::kKill,               &CommandHandler::kill               },
    {cmd::kInteract,           &CommandHandler::interact           },
    {cmd::kNarrate,            &CommandHandler::narrate            },
    {cmd::kRest,               &CommandHandler::rest               },
    {cmd::kRentRoomCheckIn,    &CommandHandler::rentRoomCheckIn    },
    {cmd::kRentRoomCheckOut,   &CommandHandler::rentRoomCheckOut   },
    {cmd::kBeginBossFight,     &CommandHandler::beginBossFight     },
    {cmd::kEndBossFight,       &CommandHandler::endBossFight       },
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
    setError(string_util::format("usage: %s <quest>", args[0].c_str()));
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
    setError(string_util::format("usage: %s <quest> <stage_idx>", args[0].c_str()));
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
    setError(string_util::format("usage: %s <itemName> [amount]", args[0].c_str()));
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
    setError(string_util::format("usage: %s <itemName> [amount]", args[0].c_str()));
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

void CommandHandler::addGold(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("usage: %s <amount>", args[0].c_str()));
    return;
  }

  int amount = 1;
  try {
    amount = std::stoi(args[1]);
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

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  player->addGold(amount);
  setSuccess();
}

void CommandHandler::removeGold(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("usage: %s <amount>", args[0].c_str()));
    return;
  }

  int amount = 1;
  try {
    amount = std::stoi(args[1]);
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

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  if (player->getGoldBalance() < amount) {
    setError("Failed to remove gold, insufficient gold.");
    return;
  }
  player->removeGold(amount);
  setSuccess();
}

void CommandHandler::updateDialogueTree(const vector<string>& args) {
  if (args.size() < 3) {
    setError(string_util::format("usage: %s <npcJson> <dialogueTreeJson>", args[0].c_str()));
    return;
  }

  // TODO: Maybe add some argument check here?

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  dialogueMgr->setLatestNpcDialogueTree(args[1], args[2]);
  setSuccess();
}

void CommandHandler::joinPlayerParty(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

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

void CommandHandler::leavePlayerParty(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

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

void CommandHandler::partyMemberWait(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFilePath)) {
    setError("This Npc is already waiting for player.");
    return;
  }

  player->getParty()->askMemberToWait(targetNpc);
  setSuccess();
}

void CommandHandler::partyMemberFollow(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();

  Player* player = gmMgr->getPlayer();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  assert(player != nullptr && targetNpc != nullptr);

  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError("This Npc has not joined player's party yet.");
    return;
  }

  if (!player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFilePath)) {
    setError("This Npc is not waiting for player yet.");
    return;
  }

  player->getParty()->askMemberToFollow(targetNpc);
  setSuccess();
}

void CommandHandler::trade(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

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

void CommandHandler::kill(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

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
    setError(string_util::format("usage: %s <narratives...>", args[0].c_str()));
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

void CommandHandler::rest(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("usage: %s", args[0].c_str()));
    return;
  }

  auto shade = SceneManager::the().getCurrentScene<GameScene>()->getShade();
  shade->getImageView()->runAction(Sequence::create(
      FadeIn::create(Shade::kFadeInTime * 3),
      FadeOut::create(Shade::kFadeOutTime * 3),
      CallFunc::create([]() {
        auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
        notifications->show("You awaken feeling well rested.");
      }),
      nullptr
  ));

  setSuccess();
}

void CommandHandler::rentRoomCheckIn(const vector<string>& args) {
  if (args.size() < 4) {
    setError(string_util::format("usage: %s <tmxMapFilePath> <bedroomKeyJsonFilePath> <gold>", args[0].c_str()));
    return;
  }

  int fee = 0;
  try {
    fee = std::stoi(args[3]);
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

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  if (player->getGoldBalance() < fee) {
    setError("Failed to rent room, insufficient gold.");
    return;
  }

  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();
  const string& tmxMapFilePath = args[1];
  if (!roomRentalTracker->checkIn(tmxMapFilePath)) {
    VGLOG(LOG_INFO, "Already rented a room in this inn.");
    setSuccess();
    return;
  }

  const string& bedroomKeyJsonFilePath = args[2];
  player->addItem(Item::create(bedroomKeyJsonFilePath));
  player->removeGold(fee);

  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  inGameTime->runAfter(0, 10, 0, string_util::format("%s %s %s", cmd::kRentRoomCheckOut,
                                                                 tmxMapFilePath.c_str(),
                                                                 bedroomKeyJsonFilePath.c_str()));

  setSuccess();
}

void CommandHandler::rentRoomCheckOut(const vector<string>& args) {
  if (args.size() < 3) {
    setError(string_util::format("usage: %s <tmxMapFilePath> <bedroomKeyJsonFilePath>", args[0].c_str()));
    return;
  }

  const string& tmxMapFilePath = args[1];
  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();
  if (!roomRentalTracker->checkOut(tmxMapFilePath)) {
    setError("Failed to check out.");
    return;
  }

  const string& bedroomKeyJsonFilePath = args[2];
  unique_ptr<Item> item = Item::create(bedroomKeyJsonFilePath);
  if (!item) {
    setError(string_util::format("Failed to create item [%s].", bedroomKeyJsonFilePath.c_str()));
    return;
  }
  Key* bedroomKey = dynamic_cast<Key*>(item.get());
  if (!bedroomKey) {
    setError(string_util::format("Failed to cast item [%s] to key.", bedroomKeyJsonFilePath.c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->setOpened(tmxMapFilePath,
                   GameMap::OpenableObjectType::PORTAL,
                   bedroomKey->getKeyProfile().targetPortalId,
                   false);

  auto player = gmMgr->getPlayer();
  player->removeItem(bedroomKey);

  setSuccess();
}

void CommandHandler::beginBossFight(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("usage: %s <bossStageProfileJsonPath>", args[0].c_str()));
    return;
  }

  const rapidjson::Document json = json_util::loadFromFile(args[1]);
  const string targetNpcJsonFilePath = json["targetNpcJsonFilePath"].GetString();
  const string bgmFilePath = json["bgmFilePath"].GetString();

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getGameMap()->onBossFightBegin(targetNpcJsonFilePath, bgmFilePath)) {
    setError(string_util::format("Failed to begin boss fight, bossStageProfileJsonPath: [%s]", args[1].c_str()));
    return;
  }

  setSuccess();
}

void CommandHandler::endBossFight(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("usage: %s <bossStageProfileJsonPath>", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getGameMap()->onBossFightEnd();

  setSuccess();
}

}  // namespace vigilante
