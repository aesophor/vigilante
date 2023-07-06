// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Window.h"

#include <cassert>

#include "Assets.h"
#include "ui/TableLayout.h"
#include "util/Logger.h"

#define DEFAULT_TITLE "Window Title"
#define DEFAULT_ROW_HEIGHT 2
#define TITLE_LABEL_OFFSET_Y -10

#define CONTENT_MARGIN_LEFT 10
#define CONTENT_MARGIN_RIGHT 10
#define CONTENT_MARGIN_TOP 25
#define CONTENT_MARGIN_BOTTOM 30

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

Window::Window(float width, float height)
    : _layer(Layer::create()),
      _layout(TableLayout::create(width, DEFAULT_ROW_HEIGHT)),
      _contentLayout(ui::Layout::create()),
      _titleLabel(Label::createWithTTF(DEFAULT_TITLE, string{kBoldFont}, kRegularFontSize)),
      _contentBg(ui::ImageView::create(string{kWindowContentBg})),
      _topLeftBg(ui::ImageView::create(string{kWindowTopLeftBg})),
      _topRightBg(ui::ImageView::create(string{kWindowTopRightBg})),
      _bottomLeftBg(ui::ImageView::create(string{kWindowBottomLeftBg})),
      _bottomRightBg(ui::ImageView::create(string{kWindowBottomRightBg})),
      _topBg(ui::ImageView::create(string{kWindowTopBg})),
      _leftBg(ui::ImageView::create(string{kWindowLeftBg})),
      _rightBg(ui::ImageView::create(string{kWindowRightBg})),
      _bottomBg(ui::ImageView::create(string{kWindowBottomBg})),
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

void Window::move(const Vec2& position) {
  setPosition(position);
  normalize();
}

void Window::move(float x, float y) {
  setPosition({x, y});
  normalize();
}

void Window::resize(const Size& size) {
  setSize(size);
  normalize();
}

void Window::resize(float width, float height) {
  setSize({width, height});
  normalize();
}

void Window::setTitle(const string& title) {
  _titleLabel->setString(title);
  normalize();
}

void Window::setVisible(bool visible) {
  _isVisible = visible;
  _layer->setVisible(visible);
}

void Window::normalize(bool init) {
  TableLayout* layout = dynamic_cast<TableLayout*>(_layout);
  layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)

  // Copy all children to another ax::Vector
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
}

}  // namespace vigilante
