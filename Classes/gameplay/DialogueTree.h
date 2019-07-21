// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_TREE_H_
#define VIGILANTE_DIALOGUE_TREE_H_

#include <string>
#include <vector>

namespace vigilante {

class DialogueTree {
 public:
  DialogueTree();
  virtual ~DialogueTree() = default;

  struct Node {
    Node(const std::string& dialog);
    std::vector<std::pair<std::string, Node*>> children;
    std::string dialog;
  };

  DialogueTree::Node* getRootNode() const;
  DialogueTree::Node* getCurrentNode() const;
  void resetCurrentNode();

 private:
  DialogueTree::Node* _rootNode;
  DialogueTree::Node* _currentNode;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_TREE_H_
