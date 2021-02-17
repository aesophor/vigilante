// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TableLayout.h"

#include "util/Logger.h"

using cocos2d::Vec2;
using cocos2d::Node;
using cocos2d::ui::Layout;

namespace vigilante {

TableLayout* TableLayout::create(float tableWidth, float rowHeight) {
  TableLayout* tableLayout = new (std::nothrow) TableLayout;
  if (tableLayout && tableLayout->init(tableWidth, rowHeight)) {
    tableLayout->autorelease();
    return tableLayout;
  }
  CC_SAFE_DELETE(tableLayout);
  return nullptr;
}

bool TableLayout::init(float tableWidth, float rowHeight) {
  if (!Layout::init()) {
    return false;
  }
  _tableWidth = tableWidth;
  _rowHeight = rowHeight;
  reset();
  return true;
}


void TableLayout::removeAllChildren() {
  Layout::removeAllChildren();
  reset();
}

void TableLayout::addChild(Node* child) {
  Layout::addChild(child);
  child->setAnchorPoint({0, 1});
  child->setPosition(_nextChildPosition);
  
  _lastAddedChild = child;
  _nextChildPosition.x += child->getScaleX() * child->getContentSize().width;
}

TableLayout* TableLayout::align(TableLayout::Alignment direction) {
  // If there's no last added child node, don't do anything.
  if (!_lastAddedChild) {
    return this;
  }

  switch (direction) {
    case Alignment::LEFT:
      _lastAddedChild->setAnchorPoint({0, 1});
      _lastAddedChild->setPositionX(0);
      _nextChildPosition.x
        = 0 + _lastAddedChild->getScaleX() * _lastAddedChild->getContentSize().width;
      break;
    case Alignment::CENTER:
      _lastAddedChild->setAnchorPoint({0.5, 1});
      _lastAddedChild->setPositionX(_tableWidth / 2);
      _nextChildPosition.x
        = _tableWidth / 2 + _lastAddedChild->getScaleX() * _lastAddedChild->getContentSize().width / 2;
      break;
    case Alignment::RIGHT:
      _lastAddedChild->setAnchorPoint({1, 1});
      _lastAddedChild->setPositionX(_tableWidth);
      _nextChildPosition.x = _tableWidth;
      break;
    default:
      VGLOG(LOG_ERR, "Bad align value: %d", static_cast<int>(direction));
      break;
  }
  return this;
}

TableLayout* TableLayout::padLeft(float padding) {
  if (!_lastAddedChild) {
    return this;
  }
  float originalX = _lastAddedChild->getPositionX();
  float newX = originalX + padding;
  float width = _lastAddedChild->getScaleX() * _lastAddedChild->getContentSize().width;
  _lastAddedChild->setPositionX(newX);
  _nextChildPosition.x = newX + width;
  return this;
}

TableLayout* TableLayout::padRight(float padding) {
  if (!_lastAddedChild) {
    return this;
  }
  float originalX = _lastAddedChild->getPositionX();
  float newX = originalX - padding;
  float width = _lastAddedChild->getScaleX() * _lastAddedChild->getContentSize().width;
  _lastAddedChild->setPositionX(newX);
  _nextChildPosition.x = newX + width;
  return this;
}

TableLayout* TableLayout::padTop(float padding) {
  if (!_lastAddedChild) {
    return this;
  }
  float originalY = _lastAddedChild->getPositionY();
  float newY = originalY - padding;
  _lastAddedChild->setPositionY(newY);
  // We don't want to update _nextChildPosition.y here,
  // otherwise the subsequently added children will placed lower and lower.
  return this;
}

TableLayout* TableLayout::padBottom(float padding) {
  if (!_lastAddedChild) {
    return this;
  }
  float originalY = _lastAddedChild->getPositionY();
  float newY = originalY + padding;
  _lastAddedChild->setPositionY(newY);
  // We don't want to update _nextChildPosition.y here,
  // otherwise the subsequently added children will placed higher and higher.
  return this;
}


TableLayout* TableLayout::spaceX(float spacing) {
  _nextChildPosition.x += spacing;
  return this;
}

TableLayout* TableLayout::spaceY(float spacing) {
  _nextChildPosition.y += spacing;
  return this;
}


TableLayout* TableLayout::row(float height) {
  _nextChildPosition.x = 0;
  _nextChildPosition.y -= height;
  return this;
}

TableLayout* TableLayout::row() {
  return row(_rowHeight);
}

void TableLayout::reset() {
  _lastAddedChild = nullptr;
  _nextChildPosition = {0, 0};
}


float TableLayout::getTableWidth() const {
  return _tableWidth;
}

float TableLayout::getRowHeight() const {
  return _rowHeight;
}

Vec2 TableLayout::getNextChildPosition() const {
  return _nextChildPosition;
}

float TableLayout::getNextChildPositionX() const {
  return _nextChildPosition.x;
}

float TableLayout::getNextChildPositionY() const {
  return _nextChildPosition.y;
}


void TableLayout::setTableWidth(float tableWidth) {
  _tableWidth = tableWidth;
}

void TableLayout::setRowHeight(float rowHeight) {
  _rowHeight = rowHeight;
}

void TableLayout::setNextChildPosition(const Vec2& pos) {
  _nextChildPosition = pos;
}

void TableLayout::setNextChildPositionX(float x) {
  _nextChildPosition.x = x;
}

void TableLayout::setNextChildPositionY(float y) {
  _nextChildPosition.y = y;
}

}  // namespace vigilante
