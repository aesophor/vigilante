// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

#include <cocos2d.h>

using std::string;

namespace vigilante {

DialogueTree::DialogueTree(const string& jsonFileName)
    : _rootNode(), _currentNode() {
  import(jsonFileName);
}

DialogueTree::~DialogueTree() {

}


void DialogueTree::import(const string& jsonFileName) {
  cocos2d::log("importing dialogue tree...");
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


DialogueTree::Node::Node(const string& dialog) : dialog(dialog) {}

} // namespace vigilante
