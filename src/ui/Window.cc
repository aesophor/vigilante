// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Window.h"

#include <cassert>

#include "AssetManager.h"
#include "ui/TableLayout.h"
#include "util/Logger.h"

#define DEFAULT_TITLE "Window Title"
#define DEFAULT_ROW_HEIGHT 2
#define TITLE_LABEL_OFFSET_Y -10

using std::string;
using cocos2d::Vec2;
using cocos2d::Size;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::Director;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kRegularFont;
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

Window::Window(float x, float y, float width, float height)
    : _layer(Layer::create()),
      _layout(TableLayout::create(width, DEFAULT_ROW_HEIGHT)),
      _contentLayout(),
      _titleLabel(Label::createWithTTF(DEFAULT_TITLE, kRegularFont, kRegularFontSize)),
      _contentBg(ImageView::create(kWindowContentBg)),
      _topLeftBg(ImageView::create(kWindowTopLeftBg)),
      _topRightBg(ImageView::create(kWindowTopRightBg)),
      _bottomLeftBg(ImageView::create(kWindowBottomLeftBg)),
      _bottomRightBg(ImageView::create(kWindowBottomRightBg)),
      _topBg(ImageView::create(kWindowTopBg)),
      _leftBg(ImageView::create(kWindowLeftBg)),
      _rightBg(ImageView::create(kWindowRightBg)),
      _bottomBg(ImageView::create(kWindowBottomBg)),
      _position(x, y),
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


  TableLayout* layout = dynamic_cast<TableLayout*>(_layout);
  const float cornerBgSideLength = _topLeftBg->getContentSize().width;
  const Size contentBgSize = {_size.width - cornerBgSideLength * 2,
                              _size.height - cornerBgSideLength * 2};

  layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)
  layout->setPosition(_position);

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

  _titleLabel->getFontAtlas()->setAliasTexParameters();
  _layer->addChild(layout);
  _layer->addChild(_titleLabel);

  placeAtCenter();
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


void Window::setContentLayout(Layout* contentLayout) {
  assert(contentLayout != nullptr);

  if (_contentLayout) {
    _layer->removeChild(_contentLayout);
  }
  _contentLayout = contentLayout;
  _layer->addChild(contentLayout);
}

void Window::setTitle(const string& title) {
  _titleLabel->setString(title);
}


const Vec2& Window::getPosition() const {
  return _position;
}

const Size& Window::getSize() const {
  return _size;
}

void Window::setPosition(const Vec2& position) {
  _position = position;
}

void Window::setPosition(float x, float y) {
  _position.x = x;
  _position.y = y;
}

void Window::setSize(const Size& size) {
  _size = size;
}

void Window::setSize(float width, float height) {
  _size.width = width;
  _size.height = height;
}

void Window::placeAtCenter() {
  auto winSize = Director::getInstance()->getWinSize();

  // Place the window itself at the center.
  _layout->setAnchorPoint({0, 1});  // Make top right (0, 0)
  _position.x = winSize.width / 2 - _size.width / 2;
  _position.y = winSize.height / 2 + _size.height / 2;
  _layout->setPosition(_position);

  // Place the title label slightly below the window's upper edge.
  float titleX = _position.x + _size.width / 2;
  float titleY = _position.y + TITLE_LABEL_OFFSET_Y;
  _titleLabel->setPosition(titleX, titleY);
}

}  // namespace vigilante
