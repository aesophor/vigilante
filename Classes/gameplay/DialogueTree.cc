// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <stack>

#include <cocos2d.h>
#include "util/JsonUtil.h"

using std::pair;
using std::stack;
using std::string;
using rapidjson::Document;

namespace vigilante {

DialogueTree::DialogueTree(const string& jsonFileName)
    : _rootNode(), _currentNode() {
  import(jsonFileName);
}

DialogueTree::~DialogueTree() {
  if (!_rootNode) {
    return;
  }
  dfsDeleteNodes(_rootNode);
}

void DialogueTree::dfsDeleteNodes(DialogueTree::Node* node) const {
  if (!node) {
    return;
  }
  
  for (auto child : node->children) {
    dfsDeleteNodes(child);
  }
  delete node;
}


void DialogueTree::import(const string& jsonFileName) {
  cocos2d::log("[%s] Importing dialogue tree...", __FILE__);
  Document json = json_util::parseJson(jsonFileName);

  // Deserialize json into runtime DialogueTree using tree DFS.
  // DFS 大師 !!!!!!! XDDDDDDDDD
  stack<pair<rapidjson::Value::Object, Node*>> st; // <jsonObject, parent>
  st.push({json["root"].GetObject(), nullptr});

  while (!st.empty()) {
    rapidjson::Value::Object node = st.top().first;
    Node* parent = st.top().second;
    st.pop();

    // Construct this node.
    _currentNode = new Node(node["dialogue"].GetString());
    if (!_rootNode) {
      _rootNode = _currentNode;
    }
    if (parent) {
      parent->children.push_back(_currentNode);
    }
   
    // Push this node's children onto the stack.
    for (auto& child : node["children"].GetArray()) {
      st.push({child.GetObject(), _currentNode});
    }
  }
}

DialogueTree::Node* DialogueTree::getRootNode() const {
  return _rootNode;
}

DialogueTree::Node* DialogueTree::getCurrentNode() const {
  return _currentNode;
}

void DialogueTree::resetCurrentNode() {
  _currentNode = _rootNode;
}


DialogueTree::Node::Node(const string& dialogue) : dialogue(dialogue), children() {}

} // namespace vigilante
