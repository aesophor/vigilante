// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "DialogueTree.h"

#include <cassert>
#include <stack>

#include <axmol.h>

#include "character/Npc.h"
#include "ui/console/CommandHandler.h"
#include "util/ds/Algorithm.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;

namespace requiem {

DialogueTree::DialogueTree(DialogueTree&& other) noexcept
    : _nodeMapper{std::move(other._nodeMapper)},
      _rootNode{std::move(other._rootNode)},
      _currentNode{other._currentNode},
      _toggleJoinPartyNode{other._toggleJoinPartyNode},
      _toggleWaitNode{other._toggleWaitNode},
      _tradeNode{other._tradeNode},
      _owner{other._owner} {}

DialogueTree& DialogueTree::operator=(DialogueTree&& other) noexcept {
  _nodeMapper = std::move(other._nodeMapper);
  _rootNode = std::move(other._rootNode);
  _currentNode = other._currentNode;
  _toggleJoinPartyNode = other._toggleJoinPartyNode;
  _toggleWaitNode = other._toggleWaitNode;
  _tradeNode = other._tradeNode;
  _isQuestDialogueTree = other._isQuestDialogueTree;
  _owner = other._owner;
  return *this;
}

void DialogueTree::import(const fs::path& jsonFilePath) {
  if (jsonFilePath.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading dialogue tree...");
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  // Convert rapidjson tree into our DialogueTree using tree DFS.
  // DFS 大師 !!!!!!! XDDDDDDDDD
  stack<tuple<rapidjson::Value::Object, Node*, Node::Type>> st;  // <jsonObject, parent>
  st.push({json.GetObject(), nullptr, Node::Type::CHILDREN});

  while (!st.empty()) {
    rapidjson::Value::Object jsonNode = std::get<0>(st.top());
    DialogueTree::Node* parent = std::get<1>(st.top());
    const Node::Type nodeType = std::get<2>(st.top());

    st.pop();

    auto node = std::make_unique<DialogueTree::Node>(this);

    if (jsonNode.HasMember("nodeName")) {
      node->_nodeName = jsonNode["nodeName"].GetString();
      _nodeMapper.insert({node->_nodeName, node.get()});
    }

    for (const auto& line : jsonNode["lines"].GetArray()) {
      node->_lines.push_back(line.GetString());
    }
    for (const auto& cmd : jsonNode["exec"].GetArray()) {
      node->_cmds.push_back(cmd.GetString());
    }

    if (jsonNode.HasMember("childrenRef")) {
      node->_childrenRef = jsonNode["childrenRef"].GetString();
    } else {
      // Push this node's children onto the stack in reverse order.
      const auto& children = jsonNode["children"].GetArray();
      for (int i = children.Size() - 1; i >= 0; i--) {
        st.push({children[i].GetObject(), node.get(), Node::Type::CHILDREN});
      }

      if (jsonNode.HasMember("childrenOnExecFail")) {
        const auto& childrenOnExecFail = jsonNode["childrenOnExecFail"].GetArray();
        for (int i = childrenOnExecFail.Size() - 1; i >= 0; i--) {
          st.push({childrenOnExecFail[i].GetObject(), node.get(), Node::Type::CHILDREN_ON_EXEC_FAIL});
        }
      }
    }

    if (!_rootNode) {
      _rootNode = std::move(node);
      _currentNode = _rootNode.get();
    } else {
      assert(parent != nullptr);
      switch (nodeType) {
        case Node::Type::CHILDREN_ON_EXEC_FAIL:
          parent->_childrenOnExecFail.emplace_back(std::move(node));
          break;
        case Node::Type::CHILDREN:
        default:
          parent->_children.emplace_back(std::move(node));
          break;
      }
    }
  }

  _isQuestDialogueTree = json["isQuestDialogueTree"].GetBool();
  if (!_isQuestDialogueTree) {
    if (_owner->getNpcProfile().isRecruitable) {
      addAllyDialogueToRootNode();
    }
    if (_owner->getNpcProfile().isTradable) {
      addTradingDialogue();
    }
  }

  update();
}

void DialogueTree::addAllyDialogueToRootNode() {
  // (1) toggle join/leave (recruit/dismiss) party
  // (2) toggle wait/follow (if this Npc already belongs to a party)
  auto toggleJoinPartyNode = std::make_unique<DialogueTree::Node>(this);
  toggleJoinPartyNode->_lines.resize(1);
  toggleJoinPartyNode->_cmds.resize(1);

  _toggleJoinPartyNode = toggleJoinPartyNode.get();
  _rootNode->_children.push_back(std::move(toggleJoinPartyNode));

  if (_owner->getParty()) {
    auto toggleWaitNode = std::make_unique<DialogueTree::Node>(this);
    toggleWaitNode->_lines.resize(1);
    toggleWaitNode->_cmds.resize(1);

    _toggleWaitNode = toggleWaitNode.get();
    _rootNode->_children.push_back(std::move(toggleWaitNode));
  }
}

void DialogueTree::addTradingDialogue() {
  auto tradeNode = std::make_unique<DialogueTree::Node>(this);
  tradeNode->_lines.push_back("Trade.");
  tradeNode->_cmds.push_back(cmd::kTrade);

  _tradeNode = tradeNode.get();
  _rootNode->_children.push_back(std::move(tradeNode));
}

void DialogueTree::update() {
  if (_toggleJoinPartyNode) {
    if (!_owner->isPlayerLeaderOfParty()) {
      _toggleJoinPartyNode->_lines.front() = "Follow me.";
      _toggleJoinPartyNode->_cmds.front() = cmd::kJoinPlayerParty;
    } else {
      _toggleJoinPartyNode->_lines.front() = "It's time for us to part ways";
      _toggleJoinPartyNode->_cmds.front() = cmd::kLeavePlayerParty;
    }
  }

  if (_toggleWaitNode) {
    if (!_owner->isWaitingForPlayer()) {
      _toggleWaitNode->_lines.front() = "Wait here.";
      _toggleWaitNode->_cmds.front() = cmd::kPartyMemberWait;
    } else {
      _toggleWaitNode->_lines.front() = "Continue to follow me.";
      _toggleWaitNode->_cmds.front() = cmd::kPartyMemberFollow;
    }
  }
}

DialogueTree::Node* DialogueTree::getNode(const string& nodeName) const {
  auto it = _nodeMapper.find(nodeName);
  return it == _nodeMapper.end() ? nullptr : it->second;
}

vector<DialogueTree::Node*> DialogueTree::Node::getChildren() const {
  if (_childrenRef.empty()) {
    return uniqueVec2RawVec<Dialogue>(_children);
  }

  DialogueTree::Node* refNode = _tree->getNode(_childrenRef);
  return uniqueVec2RawVec<Dialogue>(refNode->_children);
}

vector<DialogueTree::Node*> DialogueTree::Node::getChildrenOnExecFail() const {
  if (_childrenRef.empty()) {
    return uniqueVec2RawVec<Dialogue>(_childrenOnExecFail);
  }

  DialogueTree::Node* refNode = _tree->getNode(_childrenRef);
  return uniqueVec2RawVec<Dialogue>(refNode->_childrenOnExecFail);
}


}  // namespace requiem
