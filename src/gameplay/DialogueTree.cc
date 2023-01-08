// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <cassert>
#include <stack>

#include <cocos2d.h>

#include "character/Npc.h"
#include "util/ds/Algorithm.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;

namespace vigilante {

DialogueTree::DialogueTree(const string& jsonFileName, Npc* owner)
    : _nodeMapper(),
      _rootNode(),
      _currentNode(),
      _toggleJoinPartyNode(),
      _toggleWaitNode(),
      _tradeNode(),
      _isQuestDialogueTree(),
      _owner(owner) {
  import(jsonFileName);
}

DialogueTree::DialogueTree(DialogueTree&& other) noexcept
    : _nodeMapper(std::move(other._nodeMapper)),
      _rootNode(std::move(other._rootNode)),
      _currentNode(other._currentNode),
      _toggleJoinPartyNode(other._toggleJoinPartyNode),
      _toggleWaitNode(other._toggleWaitNode),
      _tradeNode(other._tradeNode),
      _isQuestDialogueTree(),
      _owner(other._owner) {}

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

void DialogueTree::import(const string& jsonFileName) {
  if (jsonFileName.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading dialogue tree...");
  rapidjson::Document json = json_util::parseJson(jsonFileName);

  // Convert rapidjson tree into our DialogueTree using tree DFS.
  // DFS 大師 !!!!!!! XDDDDDDDDD
  stack<pair<rapidjson::Value::Object, Node*>> st;  // <jsonObject, parent>
  st.push({json.GetObject(), nullptr});

  while (!st.empty()) {
    rapidjson::Value::Object jsonNode = st.top().first;
    DialogueTree::Node* parent = st.top().second;
    st.pop();

    // Construct this node.
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
        st.push({children[i].GetObject(), node.get()});
      }
    }

    if (!_rootNode) {
      _rootNode = std::move(node);
      _currentNode = _rootNode.get();
    } else {
      assert(parent != nullptr);
      parent->_children.push_back(std::move(node));
    }
  }


  // What's the effect of a "QuestDialogueTree"?
  // e.g., if `_isQuestDialogue` == ture, then the dialogue to
  //       toggle following/dismiss won't be present.
  _isQuestDialogueTree = json["isQuestDialogueTree"].GetBool();

  if (_isQuestDialogueTree) {
    return;
  }

  // If the dialogue tree's owner is a recruitable Npc,
  // then add the following DialogueTree::Nodes as root node's children.
  // (1) toggle join/leave (recruit/dismiss) party
  // (2) toggle wait/follow (if this Npc already belongs to a party)
  if (_owner->getNpcProfile().isRecruitable) {
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

  // If the dialogue tree's owner is a tradable Npc,
  // then add trade dialogue as a root node's child.
  if (_owner->getNpcProfile().isTradable) {
    auto tradeNode = std::make_unique<DialogueTree::Node>(this);
    tradeNode->_lines.push_back("Let's trade.");
    tradeNode->_cmds.push_back("tradeWithPlayer");
    _tradeNode = tradeNode.get();
    _rootNode->_children.push_back(std::move(tradeNode));
  }

  update();
}

void DialogueTree::update() {
  if (_toggleJoinPartyNode) {
    if (!_owner->isPlayerLeaderOfParty()) {
      _toggleJoinPartyNode->_lines.front() = "Follow me.";
      _toggleJoinPartyNode->_cmds.front() = "joinPlayerParty";
    } else {
      _toggleJoinPartyNode->_lines.front() = "It's time for us to part ways";
      _toggleJoinPartyNode->_cmds.front() = "leavePlayerParty";
    }
  }

  if (_toggleWaitNode) {
    if (!_owner->isWaitingForPlayer()) {
      _toggleWaitNode->_lines.front() = "Wait here.";
      _toggleWaitNode->_cmds.front() = "playerPartyMemberWait";
    } else {
      _toggleWaitNode->_lines.front() = "Continue to follow me.";
      _toggleWaitNode->_cmds.front() = "playerPartyMemberFollow";
    }
  }
}

DialogueTree::Node* DialogueTree::getNode(const string& nodeName) const {
  auto it = _nodeMapper.find(nodeName);
  return it == _nodeMapper.end() ? nullptr : it->second;
}

DialogueTree::Node::Node(DialogueTree* tree)
    : _tree(tree), _nodeName(), _lines(), _cmds(), _children() {}

vector<DialogueTree::Node*> DialogueTree::Node::getChildren() const {
  if (_childrenRef.empty()) {
    return uniqueVec2RawVec<Dialogue>(_children);
  }

  DialogueTree::Node* refNode = _tree->getNode(_childrenRef);
  return uniqueVec2RawVec<Dialogue>(refNode->_children);
}

}  // namespace vigilante
