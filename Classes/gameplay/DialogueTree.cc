// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueTree.h"

using std::string;

namespace vigilante {

DialogueTree::DialogueTree() : _rootNode(), _currentNode() {}

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
