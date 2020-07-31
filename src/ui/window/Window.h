// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_WINDOW_H_
#define VIGILANTE_WINDOW_H_

#include <string>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>
#include "AssetManager.h"

namespace vigilante {

class Window {
 public:
  Window(float x=150, float y=225, float width=300, float height=150);
  virtual ~Window() = default;
  
  cocos2d::Layer* getLayer() const;
  cocos2d::ui::Layout* getLayout() const;
  cocos2d::ui::Layout* getContentLayout() const;
  std::string getTitle() const;
  bool isVisible() const;
  const cocos2d::Vec2& getPosition() const;
  const cocos2d::Size& getSize() const;

  void setContentLayout(cocos2d::ui::Layout* contentLayout);
  void setTitle(const std::string& title);
  void setVisible(bool visible);
  void setPosition(const cocos2d::Vec2& position);
  void setPosition(float x, float y);
  void setSize(const cocos2d::Size& size);
  void setSize(float width, float height);

 private:
  // Place the window at the center, and place `_titleLabel` as well as
  // `_contentLayout` at the correct position.
  void normalize();

  cocos2d::Layer* _layer;

  cocos2d::ui::Layout* _layout;
  cocos2d::ui::Layout* _contentLayout;
  cocos2d::Label* _titleLabel;

  cocos2d::ui::ImageView* _contentBg;
  cocos2d::ui::ImageView* _topLeftBg;
  cocos2d::ui::ImageView* _topRightBg;
  cocos2d::ui::ImageView* _bottomLeftBg;
  cocos2d::ui::ImageView* _bottomRightBg;
  cocos2d::ui::ImageView* _topBg;
  cocos2d::ui::ImageView* _leftBg;
  cocos2d::ui::ImageView* _rightBg;
  cocos2d::ui::ImageView* _bottomBg;
  

  bool _isVisible;
  cocos2d::Vec2 _position;
  cocos2d::Size _size;
};

}  // namespace vigilante

#endif  // VIGILANTE_WINDOW_H_
