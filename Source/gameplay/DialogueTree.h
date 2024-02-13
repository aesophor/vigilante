// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_TREE_H_
#define VIGILANTE_DIALOGUE_TREE_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Importable.h"

namespace vigilante {

class Npc;

class DialogueTree : public Importable {
 public:
  DialogueTree(const std::string& jsonFilePath, Npc* owner) : _owner{owner} {
    import(jsonFilePath);
  }
  DialogueTree(const DialogueTree&) = delete;
  DialogueTree& operator=(const DialogueTree&) = delete;
  DialogueTree(DialogueTree&& other) noexcept;
  DialogueTree& operator=(DialogueTree&& other) noexcept;

  virtual ~DialogueTree() override = default;

  virtual void update();

  class Node final {
   public:
    enum class Type {
      CHILDREN,
      CHILDREN_ON_EXEC_FAIL,
    };

    explicit Node(DialogueTree* tree) : _tree{tree} {}

    inline const std::string& getNodeName() const { return _nodeName; }
    inline const std::vector<std::string>& getLines() const { return _lines; }
    inline const std::vector<std::string>& getCmds() const { return _cmds; }
    inline const std::string& getChildrenRef() const { return _childrenRef; }
    std::vector<Node*> getChildren() const;
    std::vector<Node*> getChildrenOnExecFail() const;

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
    std::vector<std::unique_ptr<Node>> _childrenOnExecFail;

    friend class DialogueTree;
  };


  virtual void import(const std::string& jsonFilePath) override;  // Importable

  DialogueTree::Node* getNode(const std::string& nodeName) const;

  inline DialogueTree::Node* getRootNode() const { return _rootNode.get(); }
  inline DialogueTree::Node* getCurrentNode() const { return _currentNode; }
  inline void setCurrentNode(DialogueTree::Node* node) { _currentNode = node; }
  inline void resetCurrentNode() { _currentNode = _rootNode.get(); }

 private:
  void addAllyDialogueToRootNode();
  void addTradingDialogue();

  // <nodeName, DialogueTree::Node*>
  std::unordered_map<std::string, DialogueTree::Node*> _nodeMapper;
  std::unique_ptr<DialogueTree::Node> _rootNode;
  DialogueTree::Node* _currentNode{};

  DialogueTree::Node* _toggleJoinPartyNode{};
  DialogueTree::Node* _toggleWaitNode{};
  DialogueTree::Node* _tradeNode{};

  bool _isQuestDialogueTree{};
  Npc* _owner{};
};

// This alias improves code readability in ui/pause_menu/DialogueListView.cc
using Dialogue = DialogueTree::Node;

}  // namespace vigilante

#endif  // VIGILANTE_DIALOGUE_TREE_H_
