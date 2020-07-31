// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <cassert>
#include <stack>

#include <cocos2d.h>
#include "std/make_unique.h"
#include "character/Npc.h"
#include "util/ds/Algorithm.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using std::pair;
using std::stack;
using std::string;
using std::vector;
using std::unordered_map;
using rapidjson::Document;

namespace vigilante {

unordered_map<string, string> DialogueTree::_latestNpcDialogueTree;

DialogueTree::DialogueTree(const string& jsonFileName, Npc* owner)
    : _nodeMapper(),
      _rootNode(),
      _currentNode(),
      _toggleJoinPartyNode(),
      _isQuestDialogue(),
      _owner(owner) {
  import(jsonFileName);
}

DialogueTree::DialogueTree(DialogueTree&& other) noexcept
    : _nodeMapper(std::move(other._nodeMapper)),
      _rootNode(std::move(other._rootNode)),
      _currentNode(other._currentNode), 
      _toggleJoinPartyNode(other._toggleJoinPartyNode),
      _isQuestDialogue(),
      _owner(other._owner) {}

DialogueTree& DialogueTree::operator=(DialogueTree&& other) noexcept {
  _nodeMapper = std::move(other._nodeMapper);
  _rootNode = std::move(other._rootNode);
  _currentNode = other._currentNode;
  _toggleJoinPartyNode = other._toggleJoinPartyNode;
  _isQuestDialogue = other._isQuestDialogue;
  _owner = other._owner;
  return *this;
}


void DialogueTree::import(const string& jsonFileName) {
  if (jsonFileName.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading dialogue tree...");
  Document json = json_util::parseJson(jsonFileName);

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


  // What's the effect of a "QuestDialogue"?
  // e.g., if `_isQuestDialogue` == ture, then the dialogue to
  //       toggle following/dismiss won't be present.
  _isQuestDialogue = json["isQuestDialogue"].GetBool();

  // If the dialogue tree's owner is a recruitable Npc,
  // then add toggle follower dialogue to top-level.
  if (!_isQuestDialogue && _owner->getNpcProfile().isRecruitable) {
    auto node = std::make_unique<DialogueTree::Node>(this);
    _toggleJoinPartyNode = node.get();

    node->_lines.resize(1);
    node->_cmds.resize(1);

    _rootNode->_children.push_back(std::move(node));
    update();
  }
}

void DialogueTree::update() {
  if (!_toggleJoinPartyNode) {
    return;
  }

  if (!_owner->isInPlayerParty()) {
    _toggleJoinPartyNode->_lines.front() = "Follow me.";
    _toggleJoinPartyNode->_cmds.front() = "joinPlayerParty";
  } else {
    _toggleJoinPartyNode->_lines.front() = "It's time for us to part ways";
    _toggleJoinPartyNode->_cmds.front() = "leavePlayerParty";
  }
}


DialogueTree::Node* DialogueTree::getNode(const string& nodeName) const {
  auto it = _nodeMapper.find(nodeName);
  if (it == _nodeMapper.end()) {
    return nullptr;
  }
  return it->second;
}


DialogueTree::Node* DialogueTree::getRootNode() const {
  return _rootNode.get();
}

DialogueTree::Node* DialogueTree::getCurrentNode() const {
  return _currentNode;
}

void DialogueTree::setCurrentNode(DialogueTree::Node* node) {
  _currentNode = node;
}

void DialogueTree::resetCurrentNode() {
  _currentNode = _rootNode.get();
}


string DialogueTree::getLatestNpcDialogueTree(const string& npcJsonFileName) {
  auto it = _latestNpcDialogueTree.find(npcJsonFileName);
  return (it != _latestNpcDialogueTree.end()) ? it->second : "";
}

void DialogueTree::setLatestNpcDialogueTree(const string& npcJsonFileName,
                                            const string& dialogueTreeJsonFileName) {
  _latestNpcDialogueTree[npcJsonFileName] = dialogueTreeJsonFileName;
}



DialogueTree::Node::Node(DialogueTree* tree)
    : _tree(tree), _nodeName(), _lines(), _cmds(), _children() {}

const string& DialogueTree::Node::getNodeName() const {
  return _nodeName;
}

const vector<string>& DialogueTree::Node::getLines() const {
  return _lines;
}

const vector<string>& DialogueTree::Node::getCmds() const {
  return _cmds;
}

vector<DialogueTree::Node*> DialogueTree::Node::getChildren() const {
  if (_childrenRef.empty()) {
    return uniqueVec2RawVec<Dialogue>(_children);
  }

  DialogueTree::Node* refNode = _tree->getNode(_childrenRef);
  return uniqueVec2RawVec<Dialogue>(refNode->_children);
}

}  // namespace vigilante
