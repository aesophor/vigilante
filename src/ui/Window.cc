// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Window.h"

#include <cassert>

#include "AssetManager.h"
#include "ui/TableLayout.h"
#include "util/Logger.h"

#define DEFAULT_TITLE "Window Title"
#define DEFAULT_ROW_HEIGHT 2
#define TITLE_LABEL_OFFSET_Y -10

#define CONTENT_MARGIN_LEFT 10
#define CONTENT_MARGIN_RIGHT 10
#define CONTENT_MARGIN_TOP 25
#define CONTENT_MARGIN_BOTTOM 30

using std::string;
using cocos2d::Vec2;
using cocos2d::Size;
using cocos2d::Node;
using cocos2d::Vector;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::Director;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kWindowContentBg;
using vigilante::asset_manager::kWindowTopLeftBg;
using vigilante::asset_manager::kWindowTopRightBg;
using vigilante::asset_manager::kWindowBottomLeftBg;
using vigilante::asset_manager::kWindowBottomRightBg;
using vigilante::asset_manager::kWindowTopBg;
using vigilante::asset_manager::kWindowLeftBg;
using vigilante::asset_manager::kWindowRightBg;
using vigilante::asset_manager::kWindowBottomBg;

namespace vigilante {

Window::Window(float width, float height)
    : _layer(Layer::create()),
      _layout(TableLayout::create(width, DEFAULT_ROW_HEIGHT)),
      _contentLayout(Layout::create()),
      _titleLabel(Label::createWithTTF(DEFAULT_TITLE, kBoldFont, kRegularFontSize)),
      _contentBg(ImageView::create(kWindowContentBg)),
      _topLeftBg(ImageView::create(kWindowTopLeftBg)),
      _topRightBg(ImageView::create(kWindowTopRightBg)),
      _bottomLeftBg(ImageView::create(kWindowBottomLeftBg)),
      _bottomRightBg(ImageView::create(kWindowBottomRightBg)),
      _topBg(ImageView::create(kWindowTopBg)),
      _leftBg(ImageView::create(kWindowLeftBg)),
      _rightBg(ImageView::create(kWindowRightBg)),
      _bottomBg(ImageView::create(kWindowBottomBg)),
      _position(0, 0),  // Calculated in Window::normalize()
      _size(width, height) {

  // All {topLeft,topRight,bottomLeft,bottomRight}Bg images
  // should be squares and be of the same size.
  assert(_topLeftBg->getContentSize().width == _topLeftBg->getContentSize().height);

  assert(_topLeftBg->getContentSize().width == _topRightBg->getContentSize().width &&
         _topLeftBg->getContentSize().width == _bottomLeftBg->getContentSize().width &&
         _topLeftBg->getContentSize().width == _bottomRightBg->getContentSize().width);

  assert(_topLeftBg->getContentSize().height == _topRightBg->getContentSize().height &&
         _topLeftBg->getContentSize().height == _bottomLeftBg->getContentSize().height &&
         _topLeftBg->getContentSize().height == _bottomRightBg->getContentSize().height);

  normalize(/*init=*/true);
}

void Window::normalize(bool init) {
  TableLayout* layout = dynamic_cast<TableLayout*>(_layout);
  layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)

  // Copy all children to another cocos2d::Vector
  // so that each child's reference count gets incremented by 1.
  // (removeChild() and removeAllChild() will decrement child's refCount by 1).
  Vector<Node*> children = layout->getChildren();
  if (!init) {
    layout->removeAllChildren();
  }


  const auto winSize = Director::getInstance()->getWinSize();
  _position.x = winSize.width / 2 - _size.width / 2;
  _position.y = winSize.height / 2 + _size.height / 2;
  layout->setPosition(_position);

  const float cornerBgSideLength = _topLeftBg->getContentSize().width;
  const Size contentBgSize = {_size.width - cornerBgSideLength * 2,
                              _size.height - cornerBgSideLength * 2};

  _topBg->setScaleX(contentBgSize.width);
  _leftBg->setScaleY(contentBgSize.height);
  _contentBg->setScaleX(contentBgSize.width);
  _contentBg->setScaleY(contentBgSize.height);
  _rightBg->setScaleY(contentBgSize.height);
  _bottomBg->setScaleX(contentBgSize.width);

  layout->addChild(_topLeftBg);
  layout->addChild(_topBg);
  layout->addChild(_topRightBg);
  layout->row();

  layout->addChild(_leftBg);
  layout->addChild(_contentBg);
  layout->addChild(_rightBg);
  layout->row(_size.height - cornerBgSideLength * 2);

  layout->addChild(_bottomLeftBg);
  layout->addChild(_bottomBg);
  layout->addChild(_bottomRightBg);

  if (init) {
    _layer->addChild(layout);
  } else {
    layout->retain();
    _layer->removeChild(layout);  // child's refCount -= 1
    _layer->addChild(layout);  // child's refCount += 1
    layout->release();
  }

  
  // Reposition `_contentLayout` 
  const float contentX = _position.x + CONTENT_MARGIN_LEFT;
  const float contentY = _position.y - CONTENT_MARGIN_TOP;
  _contentLayout->setAnchorPoint({0, 1});
  _contentLayout->setPosition({contentX, contentY - 15});  // FIXME: remove this literal
 
  if (init) {
    _layer->addChild(_contentLayout);
  } else {
    _contentLayout->retain();
    _layer->removeChild(_contentLayout);  // child's refCount -= 1
    _layer->addChild(_contentLayout);  // child's refCount += 1
    _contentLayout->release();
  }


  // Place the title label slightly below the window's upper edge.
  const float titleX = _position.x + _size.width / 2;
  const float titleY = _position.y + TITLE_LABEL_OFFSET_Y;
  _titleLabel->setPosition(titleX, titleY);
  _titleLabel->getFontAtlas()->setAliasTexParameters();

  if (init) {
    _layer->addChild(_titleLabel);
  } else {
    _titleLabel->retain();
    _layer->removeChild(_titleLabel);  // child's refCount -= 1
    _layer->addChild(_titleLabel);  // child's refCount += 1
    _titleLabel->release();
  }

  /*
  for (auto child : children) {
    VGLOG(LOG_INFO, "RefCount: %d", child->getReferenceCount());
  }
  */
}


Layer* Window::getLayer() const {
  return _layer;
}

Layout* Window::getLayout() const {
  return _layout;
}

Layout* Window::getContentLayout() const {
  return _contentLayout;
}

string Window::getTitle() const {
  return _titleLabel->getString();
}

bool Window::isVisible() const {
  return _isVisible;
}

const Vec2& Window::getPosition() const {
  return _position;
}

const Size& Window::getSize() const {
  return _size;
}


void Window::setTitle(const string& title) {
  _titleLabel->setString(title);
  normalize();
}

void Window::setVisible(bool visible) {
  _isVisible = visible;
  _layer->setVisible(visible);
}


void Window::setPosition(const Vec2& position) {
  _position = position;
  normalize();
}

void Window::setPosition(float x, float y) {
  setPosition({x, y});
}

void Window::setSize(const Size& size) {
  _size = size;
  normalize();
}

void Window::setSize(float width, float height) {
  setSize({width, height});
}

}  // namespace vigilante
