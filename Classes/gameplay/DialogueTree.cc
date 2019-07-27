// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <stack>

#include <cocos2d.h>
#include "util/JsonUtil.h"
#include "util/Logger.h"

using std::pair;
using std::stack;
using std::string;
using std::vector;
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
  VGLOG(LOG_INFO, "Loading dialogue tree...");
  Document json = json_util::parseJson(jsonFileName);

  // Deserialize json into runtime DialogueTree using tree DFS.
  // DFS 大師 !!!!!!! XDDDDDDDDD
  stack<pair<rapidjson::Value::Object, Node*>> st; // <jsonObject, parent>
  st.push({json.GetObject(), nullptr});

  while (!st.empty()) {
    rapidjson::Value::Object node = st.top().first;
    Node* parent = st.top().second;
    st.pop();

    // Construct this node.
    vector<string> lines;
    for (auto& line : node["lines"].GetArray()) {
      lines.push_back(line.GetString());
    }
    _currentNode = new Node(lines);

    if (!_rootNode) {
      _rootNode = _currentNode;
    }
    if (parent) {
      parent->children.push_back(_currentNode);
    }
   
    // Push this node's children onto the stack in reverse order.
    const auto children = node["children"].GetArray();
    for (int i = children.Size() - 1; i >= 0; i--) {
      st.push({children[i].GetObject(), _currentNode});
    }
  }

  // Set _currentNode to _rootNode for later use (UI)
  _currentNode = _rootNode;
}

DialogueTree::Node* DialogueTree::getRootNode() const {
  return _rootNode;
}

DialogueTree::Node* DialogueTree::getCurrentNode() const {
  return _currentNode;
}

void DialogueTree::setCurrentNode(DialogueTree::Node* node) {
  _currentNode = node;
}

void DialogueTree::resetCurrentNode() {
  _currentNode = _rootNode;
}


DialogueTree::Node::Node(const vector<string>& lines) : lines(lines), children() {}

} // namespace vigilante
