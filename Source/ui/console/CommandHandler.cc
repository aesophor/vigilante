// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "CommandHandler.h"

#include <memory>

#include "Audio.h"
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

namespace requiem {

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
    {cmd::kEcho,               &CommandHandler::echo               },
    {cmd::kSetBgmVolume,       &CommandHandler::setBgmVolume       },
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
    {cmd::kSetInGameTime,      &CommandHandler::setInGameTime      },
    {cmd::kMoveTo,             &CommandHandler::moveTo             },
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

void CommandHandler::echo(const std::vector<std::string>& args) {
 if (args.size() < 2) {
    setError(string_util::format("Usage: %s <string>", args[0].c_str()));
    return;
  }

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show(args[1]);
  setSuccess();
}

void CommandHandler::setBgmVolume(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <volume>", args[0].c_str()));
    return;
  }

  float volume{0};
  try {
    volume = std::stof(args[1]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, volume: [%s]", args[1].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, volume: [%s]", args[1].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  if (volume < 0 || volume > 100) {
    setError("Volume must be set between [0,100]");
    return;
  }

  Audio::the().setBgmVolume(volume / 100);
  setSuccess();
}

void CommandHandler::startQuest(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <quest>", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getPlayer()->getQuestBook().startQuest(args[1])) {
    setError(string_util::format("Failed to start quest: [%s]", args[1].c_str()));
    return;
  }

  setSuccess();
}

void CommandHandler::setStage(const vector<string>& args) {
  if (args.size() < 3) {
    setError(string_util::format("Usage: %s <quest> <stage_idx>", args[0].c_str()));
    return;
  }

  int stageIdx = 0;
  try {
    stageIdx = std::stoi(args[2]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, stage_idx: [%s]", args[2].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, stage_idx: [%s]", args[2].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getPlayer()->getQuestBook().setStage(args[1], stageIdx)) {
    setError(string_util::format("failed to set the stage of quest [%s]", args[1].c_str()));
    return;
  }

  setSuccess();
}

void CommandHandler::addItem(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <itemJsonFilePath> [amount]", args[0].c_str()));
    return;
  }

  int amount = 1;
  if (args.size() >= 3) {
    try {
      amount = std::stoi(args[2]);
    } catch (const invalid_argument& ex) {
      setError(string_util::format("Invalid argument, amount: [%s]", args[2].c_str()));
      return;
    } catch (const out_of_range& ex) {
      setError(string_util::format("Out of range, amount: [%s]", args[2].c_str()));
      return;
    } catch (...) {
      setError("Unknown error");
      return;
    }
  }

  if (amount <= 0) {
    setError(string_util::format("Invalid argument, amount: [%d] has to be at least 1", amount));
    return;
  }

  unique_ptr<Item> item = Item::create(args[1]);
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getPlayer()->addItem(std::move(item), amount);
  setSuccess();
}

void CommandHandler::removeItem(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <itemJsonFilePath> [amount]", args[0].c_str()));
    return;
  }

  int amount = 1;
  if (args.size() >= 3) {
    try {
      amount = std::stoi(args[2]);
    } catch (const invalid_argument& ex) {
      setError(string_util::format("Invalid argument, amount: [%s]", args[2].c_str()));
      return;
    } catch (const out_of_range& ex) {
      setError(string_util::format("Out of range, amount: [%s]", args[2].c_str()));
      return;
    } catch (...) {
      setError("Unknown error");
      return;
    }
  }

  if (amount <= 0) {
    setError(string_util::format("Invalid argument, amount: [%d] has to be at least 1", amount));
    return;
  }

  unique_ptr<Item> item = Item::create(args[1]);
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getPlayer()->removeItem(item.get(), amount);
  setSuccess();
}

void CommandHandler::addGold(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <amount>", args[0].c_str()));
    return;
  }

  int amount = 1;
  try {
    amount = std::stoi(args[1]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, amount: [%s]", args[1].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, amount: [%s]", args[1].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  if (amount <= 0) {
    setError(string_util::format("Invalid argument, amount: [%d] has to be at least 1", amount));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  player->addGold(amount);
  setSuccess();
}

void CommandHandler::removeGold(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <amount>", args[0].c_str()));
    return;
  }

  int amount = 1;
  try {
    amount = std::stoi(args[1]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, amount: [%s]", args[1].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, amount: [%s]", args[1].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  if (amount <= 0) {
    setError(string_util::format("Invalid argument, amount: [%d] has to be at least 1", amount));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  if (player->getGoldBalance() < amount) {
    setError("Failed to remove gold, since player has insufficient gold.");
    return;
  }
  player->removeGold(amount);
  setSuccess();
}

void CommandHandler::updateDialogueTree(const vector<string>& args) {
  if (args.size() < 3) {
    setError(string_util::format("Usage: %s <npcJson> <dialogueTreeJson>", args[0].c_str()));
    return;
  }

  // TODO: Maybe add some argument check here?

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  dialogueMgr->setLatestNpcDialogueTree(args[1], args[2]);
  setSuccess();
}

void CommandHandler::joinPlayerParty(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }
  if (targetNpc->isPlayerLeaderOfParty()) {
    setError(string_util::format("Failed to make npc [%s] join the player's party, "
                                 "since it has already joined the player's party.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }

  player->getParty()->recruit(targetNpc);
  setSuccess();
}

void CommandHandler::leavePlayerParty(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }
  if (!targetNpc->isPlayerLeaderOfParty()) {
     setError(string_util::format("Failed to make npc [%s] leave the player's party, "
                                  "since it not joined player's party yet.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }

  player->getParty()->dismiss(targetNpc);
  setSuccess();
}

void CommandHandler::partyMemberWait(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }
  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError(string_util::format("Failed to make npc [%s] wait for the player, "
                                 "since it has not joined player's party yet.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }
  if (player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFilePath)) {
    setError(string_util::format("Failed to make npc [%s] wait for the player, "
                                 "since it is already waiting for the player.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }

  player->getParty()->askMemberToWait(targetNpc);
  setSuccess();
}

void CommandHandler::partyMemberFollow(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }
  if (!targetNpc->isPlayerLeaderOfParty()) {
    setError(string_util::format("Failed to make npc [%s] follow the player, "
                                 "since it has not joined player's party yet.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }
  if (!player->getParty()->getWaitingMemberLocationInfo(targetNpc->getCharacterProfile().jsonFilePath)) {
    setError(string_util::format("Failed to make npc [%s] follow the player, "
                                 "since it is not waiting for the player.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }

  player->getParty()->askMemberToFollow(targetNpc);
  setSuccess();
}

void CommandHandler::trade(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }
  if (!targetNpc->getNpcProfile().isTradable) {
    setError(string_util::format("Failed to trade with npc [%s], since it is not tradable.",
                                 targetNpc->getCharacterProfile().jsonFilePath.c_str()));
    return;
  }

  targetNpc->beginTrade();
  setSuccess();
}

void CommandHandler::kill(const vector<string>& args) {
  if (args.size() < 1) {
    setError(string_util::format("Usage: %s", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player");
    return;
  }

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Npc* targetNpc = dialogueMgr->getTargetNpc();
  if (!targetNpc) {
    setError("Failed to get target npc from dialogue manager");
    return;
  }

  targetNpc->receiveDamage(player, 99999);
  setSuccess();
}

void CommandHandler::interact(const vector<string>& args) {
  if (args.size() > 2) {
    setError(string_util::format("Usage: %s [npcJsonFilePath]", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Player* player = gmMgr->getPlayer();
  if (!player) {
    setError("Failed to get player.");
    return;
  }

  if (args.size() == 2) {
    const fs::path npcJsonFilePath{args[1]};
    for (const auto& actor : gmMgr->getGameMap()->getDynamicActors()) {
      if (auto npc = dynamic_pointer_cast<Npc>(actor)) {
        player->interact(npc.get());
        break;
      }
    }
  } else {
    const list<Interactable*>& interactables = player->getInRangeInteractables();
    if (interactables.empty()) {
      setError("Failed to interact with nearby interactable objects since there aren't any.");
      return;
    }
    for (auto interactable : interactables) {
      player->interact(interactable);
    }
  }

  setSuccess();
}

void CommandHandler::narrate(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <narratives...>", args[0].c_str()));
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
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <innTmxMapFilePath>", args[0].c_str()));
    return;
  }

  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();
  const string& tmxMapFilePath = args[1];
  if (!roomRentalTracker->hasCheckedIn(tmxMapFilePath)) {
    auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
    notifications->show("You cannot rest here because you haven't rented this room.");
    setError(string_util::format("Failed to rest in [%s], haven't checked in.", tmxMapFilePath.c_str()));
    return;
  }

  auto shade = SceneManager::the().getCurrentScene<GameScene>()->getShade();
  shade->getImageView()->runAction(Sequence::create(
      FadeIn::create(Shade::kFadeInTime * 3),
      CallFunc::create([]() {
        auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
        inGameTime->fastForward(8 * 60 * 60);  // 8 hours

        auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
        notifications->show("You awaken feeling well rested.");
      }),
      FadeOut::create(Shade::kFadeOutTime * 3),
      nullptr
  ));

  setSuccess();
}

void CommandHandler::rentRoomCheckIn(const vector<string>& args) {
  if (args.size() < 4) {
    setError(string_util::format("Usage: %s <innTmxMapFilePath> <bedroomKeyJsonFilePath> <gold>", args[0].c_str()));
    return;
  }

  int fee = 0;
  try {
    fee = std::stoi(args[3]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, fee: [%s]", args[3].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, fee: [%s]", args[3].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();
  if (player->getGoldBalance() < fee) {
    setError("Failed to rent room, since player has insufficient gold.");
    return;
  }

  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();
  const string& tmxMapFilePath = args[1];
  if (!roomRentalTracker->checkIn(tmxMapFilePath)) {
    VGLOG(LOG_INFO, "Already rented a room in this inn.");
    setSuccess();
    return;
  }

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show("You have successfully rented a room.");

  const string& bedroomKeyJsonFilePath = args[2];
  player->addItem(Item::create(bedroomKeyJsonFilePath));
  player->removeGold(fee);

  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  inGameTime->runAfter(24, 0, 0, string_util::format("%s %s %s", cmd::kRentRoomCheckOut,
                                                                 tmxMapFilePath.c_str(),
                                                                 bedroomKeyJsonFilePath.c_str()));

  setSuccess();
}

void CommandHandler::rentRoomCheckOut(const vector<string>& args) {
  if (args.size() < 3) {
    setError(string_util::format("Usage: %s <innTmxMapFilePath> <bedroomKeyJsonFilePath>", args[0].c_str()));
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
    setError(string_util::format("Usage: %s <bossStageProfileJsonPath>", args[0].c_str()));
    return;
  }

  const rapidjson::Document json = json_util::loadFromFile(args[1]);
  const string targetNpcJsonFilePath = json_util::extract<string>(json, "targetNpcJsonFilePath", "");
  const string bgmFilePath = json_util::extract<string>(json.GetObject(), "bgmFilePath", "");
  optional<string> preFightCmds = json_util::extract<string>(json.GetObject(), "preFightCmds");
  optional<string> postFightFailedCmds = json_util::extract<string>(json.GetObject(), "postFightFailedCmds");
  optional<string> postFightSuccessfulCmds = json_util::extract<string>(json.GetObject(), "postFightSuccessfulCmds");

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (!gmMgr->getGameMap()->onBossFightBegin(targetNpcJsonFilePath,
                                             bgmFilePath,
                                             std::move(preFightCmds),
                                             std::move(postFightFailedCmds),
                                             std::move(postFightSuccessfulCmds))) {
    setError(string_util::format("Failed to begin boss fight, bossStageProfileJsonPath: [%s]", args[1].c_str()));
    return;
  }

  setSuccess();
}

void CommandHandler::endBossFight(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("Usage: %s <bossStageProfileJsonPath>", args[0].c_str()));
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getGameMap()->onBossFightEnd(/*successful=*/true);

  setSuccess();
}

void CommandHandler::setInGameTime(const vector<string>& args) {
  if (args.size() < 4) {
    setError(string_util::format("Usage: %s <hour> <minute> <second>", args[0].c_str()));
    return;
  }

  int hour = 0;
  try {
    hour = std::stoi(args[1]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, hour: [%s]", args[1].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, hour: [%s]", args[1].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  int minute = 0;
  try {
    minute = std::stoi(args[2]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, minute: [%s]", args[2].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, minute: [%s]", args[2].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  int second = 0;
  try {
    second = std::stoi(args[3]);
  } catch (const invalid_argument& ex) {
    setError(string_util::format("Invalid argument, second: [%s]", args[3].c_str()));
    return;
  } catch (const out_of_range& ex) {
    setError(string_util::format("Out of range, second: [%s]", args[3].c_str()));
    return;
  } catch (...) {
    setError("Unknown error");
    return;
  }

  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  const int currentHour = inGameTime->getHour();
  const int currentMinute = inGameTime->getMinute();
  const int currentSecond = inGameTime->getSecond();

  const int secElapsedToday = currentSecond + 60 * currentMinute + 3600 * currentHour;
  int secElapsedTarget = second + 60 * minute + 3600 * hour;
  if (secElapsedTarget < secElapsedToday) {
    secElapsedTarget += 24 * 60 * 60;
  }
  secElapsedTarget -= secElapsedToday;
  inGameTime->fastForward(secElapsedTarget);

  setSuccess();
}

void CommandHandler::moveTo(const vector<string>& args) {
  if (args.size() < 2) {
    setError(string_util::format("usage: %s <mapAlias> [x] [y]", args[0].c_str()));
    return;
  }

  const string& mapAlias = args[1];
  optional<float> x;
  optional<float> y;

  if (args.size() >= 3) {
    try {
      x = std::stof(args[2]) / kPpm;
    } catch (const invalid_argument& ex) {
      setError(string_util::format("Invalid argument, x: [%s]", args[2].c_str()));
      return;
    } catch (const out_of_range& ex) {
      setError(string_util::format("Out of range, x: [%s]", args[2].c_str()));
      return;
    } catch (...) {
      setError("Unknown error");
      return;
    }
  }

  if (args.size() >= 4) {
    try {
      y = std::stof(args[3]) / kPpm;
    } catch (const invalid_argument& ex) {
      setError(string_util::format("Invalid argument, y: [%s]", args[3].c_str()));
      return;
    } catch (const out_of_range& ex) {
      setError(string_util::format("Out of range, y: [%s]", args[3].c_str()));
      return;
    } catch (...) {
      setError("Unknown error");
      return;
    }
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  const optional<string> tmxMapFilePath = gmMgr->getTmxMapFilePathByMapAlias(mapAlias);
  if (!tmxMapFilePath.has_value()) {
    setError(string_util::format("Failed to moveto unknown map alias: [%s].", mapAlias.c_str()));
    return;
  }

  auto afterLoadingGameMap = [gmMgr, x, y](const GameMap* newGameMap) {
    Player* player = gmMgr->getPlayer();
    if (!player) {
      return;
    }

    b2Vec2 newPlayerPos;
    if (x.has_value() || y.has_value()) {
      newPlayerPos = {x.value(), y.value()};
    } else {
      GameMap::Portal* portal = newGameMap->getPortals()[0].get();
      newPlayerPos = portal->getBody()->GetPosition();
    }
    player->setPosition(newPlayerPos.x, newPlayerPos.y);

    for (auto ally : player->getAllies()) {
      if (!ally->isWaitingForPartyLeader()) {
        ally->setPosition(newPlayerPos.x, newPlayerPos.y);
      } else if (newGameMap->getTmxTiledMapFilePath() != ally->getParty()->getWaitingMemberLocationInfo(
                 ally->getCharacterProfile().jsonFilePath)->tmxMapFilePath) {
        ally->removeFromMap();
      }
    }
  };

  gmMgr->loadGameMap(*tmxMapFilePath, afterLoadingGameMap);

  setSuccess();
}

}  // namespace requiem
