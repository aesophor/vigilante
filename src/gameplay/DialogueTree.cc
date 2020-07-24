// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <cassert>
#include <stack>

#include <cocos2d.h>
#include "std/make_unique.h"
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

DialogueTree::DialogueTree(const string& jsonFileName)
    : _rootNode(), _currentNode() {
  import(jsonFileName);
}

DialogueTree::DialogueTree(DialogueTree&& other) noexcept
    : _rootNode(std::move(other._rootNode)),
      _currentNode(other._currentNode) {}

DialogueTree& DialogueTree::operator=(DialogueTree&& other) noexcept {
  _rootNode = std::move(other._rootNode);
  _currentNode = other._currentNode;
  return *this;
}


void DialogueTree::import(const string& jsonFileName) {
  VGLOG(LOG_INFO, "Loading dialogue tree...");
  Document json = json_util::parseJson(jsonFileName);

  // Deserialize json into runtime DialogueTree using tree DFS.
  // DFS 大師 !!!!!!! XDDDDDDDDD
  stack<pair<rapidjson::Value::Object, Node*>> st;  // <jsonObject, parent>
  st.push({json.GetObject(), nullptr});

  while (!st.empty()) {
    rapidjson::Value::Object node = st.top().first;
    Node* parent = st.top().second;
    st.pop();

    // Construct this node.
    auto currentNode = std::make_unique<Node>();
    _currentNode = currentNode.get();

    for (const auto& line : node["lines"].GetArray()) {
      currentNode->lines.push_back(line.GetString());
    }
    for (const auto& cmd : node["exec"].GetArray()) {
      currentNode->cmds.push_back(cmd.GetString());
    }

    if (!_rootNode) {
      _rootNode = std::move(currentNode);
    } else {
      assert(parent != nullptr);
      parent->children.push_back(std::move(currentNode));
    }
   
    // Push this node's children onto the stack in reverse order.
    const auto& children = node["children"].GetArray();
    for (int i = children.Size() - 1; i >= 0; i--) {
      st.push({children[i].GetObject(), _currentNode});
    }
  }

  // Set _currentNode to _rootNode for later use (UI)
  resetCurrentNode();
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

}  // namespace vigilante
