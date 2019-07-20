// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogTree.h"

using std::string;

namespace vigilante {

DialogTree::DialogTree() : _rootNode(), _currentNode() {}

DialogTree::Node* DialogTree::getRootNode() const {
  return _rootNode;
}

DialogTree::Node* DialogTree::getCurrentNode() const {
  return _currentNode;
}

void DialogTree::resetCurrentNode() {
  _currentNode = _rootNode;
}


DialogTree::Node::Node(const string& dialog) : dialog(dialog) {}

} // namespace vigilante
