// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_WINDOW_H_
#define REQUIEM_UI_WINDOW_H_

#include <string>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

#include "Controllable.h"

namespace requiem {

class Window : public Controllable {
 public:
  Window(const float width=300, const float height=150);
  virtual ~Window() = default;

  virtual void update(const float delta) = 0;
  virtual void handleInput() = 0;  // Controllable

  virtual void move(const ax::Vec2& position);
  virtual void move(float x, float y);
  virtual void resize(const ax::Size& size);
  virtual void resize(float width, float height);
  virtual void setVisible(bool visible);

  inline ax::Layer* getLayer() const { return _layer; }
  inline ax::ui::Layout* getLayout() const { return _layout; }
  inline ax::ui::Layout* getContentLayout() const { return _contentLayout; }
  inline std::string getTitle() const { return std::string{_titleLabel->getString()}; }
  inline bool isVisible() const { return _isVisible; }
  inline const ax::Vec2& getPosition() const { return _position; }
  inline const ax::Size& getSize() const { return _size; }

  void setTitle(const std::string& title);

 protected:
  // Place the window at the center, and place `_titleLabel` as well as
  // `_contentLayout` at the correct position.
  void normalize(bool init=false);

  inline void setPosition(const ax::Vec2& position) { _position = position; }
  inline void setSize(const ax::Size& size) { _size = size; }

  ax::Layer* _layer;

  ax::ui::Layout* _layout;
  ax::ui::Layout* _contentLayout;
  ax::Label* _titleLabel;

  ax::ui::ImageView* _contentBg;
  ax::ui::ImageView* _topLeftBg;
  ax::ui::ImageView* _topRightBg;
  ax::ui::ImageView* _bottomLeftBg;
  ax::ui::ImageView* _bottomRightBg;
  ax::ui::ImageView* _topBg;
  ax::ui::ImageView* _leftBg;
  ax::ui::ImageView* _rightBg;
  ax::ui::ImageView* _bottomBg;
  
  bool _isVisible{};
  ax::Vec2 _position{0, 0};
  ax::Size _size;
};

}  // namespace requiem

#endif  // REQUIEM_UI_WINDOW_H_
