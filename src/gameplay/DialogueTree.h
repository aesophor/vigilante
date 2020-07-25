// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_TREE_H_
#define VIGILANTE_DIALOGUE_TREE_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

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


  class Node final {
   public:
    explicit Node(DialogueTree* tree);
    ~Node() = default;

    const std::string& getNodeName() const;
    const std::vector<std::string>& getLines() const;
    const std::vector<std::string>& getCmds() const;
    const std::string& getChildrenRef() const;
    std::vector<Node*> getChildren() const;

   private:
    DialogueTree* _tree;

    std::string _nodeName;  // only required when `childrenRef` exists. See comment below.
    std::vector<std::string> _lines;
    std::vector<std::string> _cmds;  // the command to execute after all lines are shown.

    // We have two (mutually exclusive) methods for keeping children:
    //
    // (a) childrenRef: we reference another node's children by its `nodeName`.
    //                               ~~~~~~~~~~~~               ~~~
    //                                    |______________________|
    // (b) children: we actually keep unique_ptr<Node> to the nodes.
    std::string _childrenRef;
    std::vector<std::unique_ptr<Node>> _children;

    friend class DialogueTree;
  };


  virtual void import(const std::string& jsonFileName) override;  // Importable

  DialogueTree::Node* getNode(const std::string& nodeName) const;
  
  DialogueTree::Node* getRootNode() const;
  DialogueTree::Node* getCurrentNode() const;
  void setCurrentNode(DialogueTree::Node* node);
  void resetCurrentNode();

  static std::string getLatestNpcDialogueTree(const std::string& npcJsonFileName);
  static void setLatestNpcDialogueTree(const std::string& npcJsonFileName,
                                       const std::string& dialogueTreeJsonFileName);

 private:
  std::unique_ptr<DialogueTree::Node> _rootNode;
  DialogueTree::Node* _currentNode;

  // <nodeName, DialogueTree::Node*>
  std::unordered_map<std::string, DialogueTree::Node*> _nodeMapper;

  static std::unordered_map<std::string, std::string> _latestNpcDialogueTree;
};


// This alias improves code readability in ui/pause_menu/DialogueListView.cc
using Dialogue = DialogueTree::Node;

}  // namespace vigilante

#endif  // VIGILANTE_DIALOGUE_TREE_H_
