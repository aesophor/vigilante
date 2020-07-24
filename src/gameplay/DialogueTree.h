// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_TREE_H_
#define VIGILANTE_DIALOGUE_TREE_H_

#include <string>
#include <vector>
#include <memory>

#include "Importable.h"

namespace vigilante {

class DialogueTree : public Importable {
 public:
  explicit DialogueTree(const std::string& jsonFileName);
  DialogueTree(const DialogueTree&) = delete;
  DialogueTree(DialogueTree&& other) noexcept;
  DialogueTree& operator=(const DialogueTree&) = delete;
  DialogueTree& operator=(DialogueTree&& other) noexcept;
  virtual ~DialogueTree() = default;

  virtual void import(const std::string& jsonFileName) override;

  struct Node {
    std::vector<std::string> lines;
    std::vector<std::string> cmds;
    std::vector<std::unique_ptr<Node>> children;
  };
 
  DialogueTree::Node* getRootNode() const;
  DialogueTree::Node* getCurrentNode() const;
  void setCurrentNode(DialogueTree::Node* node);
  void resetCurrentNode();

  static std::string getLatestNpcDialogueTree(const std::string& npcJsonFileName);
  static void setLatestNpcDialogueTree(const std::string& npcJsonFileName,
                                       const std::string& dialogueTreeJsonFileName);

 private:
  static std::unordered_map<std::string, std::string> _latestNpcDialogueTree;

  std::unique_ptr<DialogueTree::Node> _rootNode;
  DialogueTree::Node* _currentNode;
};


// This alias improves code readability in ui/pause_menu/DialogueListView.cc
using Dialogue = DialogueTree::Node;

}  // namespace vigilante

#endif  // VIGILANTE_DIALOGUE_TREE_H_
