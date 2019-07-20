// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOG_TREE_H_
#define VIGILANTE_DIALOG_TREE_H_

#include <string>
#include <vector>

namespace vigilante {

class DialogTree {
 public:
  DialogTree();
  virtual ~DialogTree() = default;

  struct Node {
    Node(const std::string& dialog);
    std::vector<std::pair<std::string, Node*>> children;
    std::string dialog;
  };

  DialogTree::Node* getRootNode() const;
  DialogTree::Node* getCurrentNode() const;
  void resetCurrentNode();

 private:
  DialogTree::Node* _rootNode;
  DialogTree::Node* _currentNode;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOG_TREE_H_
