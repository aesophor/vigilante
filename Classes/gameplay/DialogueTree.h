// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_TREE_H_
#define VIGILANTE_DIALOGUE_TREE_H_

#include <string>
#include <vector>

#include "Importable.h"

namespace vigilante {

class DialogueTree : public Importable {
 public:
  DialogueTree(const std::string& jsonFileName);
  virtual ~DialogueTree();

  virtual void import(const std::string& jsonFileName) override;

  struct Node {
    std::vector<std::string> lines;
    std::vector<std::string> cmds;
    std::vector<Node*> children;
  };
 
  DialogueTree::Node* getRootNode() const;
  DialogueTree::Node* getCurrentNode() const;
  void setCurrentNode(DialogueTree::Node* node);
  void resetCurrentNode();

 private:
  void dfsDeleteNodes(DialogueTree::Node* node) const;

  DialogueTree::Node* _rootNode;
  DialogueTree::Node* _currentNode;
};


// This alias improves code readability in ui/pause_menu/DialogueListView.cc
using Dialogue = DialogueTree::Node;

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_TREE_H_
