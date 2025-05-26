// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_PAUSE_MENU_ABSTRACT_PANE_H_
#define REQUIEM_UI_PAUSE_MENU_ABSTRACT_PANE_H_

#include <axmol.h>
#include <ui/UILayout.h>

namespace requiem {

// Forward declaration of PauseMenu class.
// We only need to know its existence, so
// we don't have to include the entire class.
class PauseMenu;

class AbstractPane {
 public:
  virtual ~AbstractPane() = default;

  virtual void update() = 0;
  virtual void handleInput() = 0;

  bool isVisible() const;
  void setVisible(bool visible) const;
  void setPosition(const ax::Vec2& pos) const;

  inline PauseMenu* getPauseMenu() const { return _pauseMenu; }
  inline ax::ui::Layout* getLayout() const { return _layout; }

 protected:
  explicit AbstractPane(PauseMenu* pauseMenu);  // install ax's UILayout
  AbstractPane(PauseMenu* pauseMenu, ax::ui::Layout* layout);  // install custom layout

  PauseMenu* _pauseMenu;
  ax::ui::Layout* _layout;  // auto-release object
};

}  // namespace requiem

#endif  // REQUIEM_UI_PAUSE_MENU_ABSTRACT_PANE_H_
