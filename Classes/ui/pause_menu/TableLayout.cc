#include "TableLayout.h"

using cocos2d::Vec2;
using cocos2d::Node;
using cocos2d::ui::Layout;

namespace vigilante {

TableLayout::TableLayout(float tableWidth, float rowHeight)
    : Layout(),
      _tableWidth(tableWidth),
      _rowHeight(rowHeight),
      _lastAddedChild(),
      _nextChildPosition({0, 0}) {
  Layout::init();
  autorelease();
}


void TableLayout::addChild(Node* child) {
  Layout::addChild(child);
  child->setPosition(_nextChildPosition);

  _lastAddedChild = child;
  _nextChildPosition.x += child->getContentSize().width;
}

TableLayout* TableLayout::align(TableLayout::Direction direction) {
  // If there's no last added child node, don't do anything.
  if (!_lastAddedChild) {
    return this;
  }

  switch (direction) {
    case Direction::LEFT:
      _lastAddedChild->setAnchorPoint({0, 1});
      _lastAddedChild->setPositionX(0);
      break;
    case Direction::CENTER:
      _lastAddedChild->setAnchorPoint({0.5, 1});
      _lastAddedChild->setPositionX(_tableWidth / 2);
      break;
    case Direction::RIGHT:
      _lastAddedChild->setAnchorPoint({1, 1});
      _lastAddedChild->setPositionX(_tableWidth);
      break;
    default:
      cocos2d::log("[TableLayout] bad align value in TableLayout::align()");
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
  float width = _lastAddedChild->getContentSize().width;
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
  float width = _lastAddedChild->getContentSize().width;
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

TableLayout* TableLayout::row(float height) {
  _nextChildPosition.x = 0;
  _nextChildPosition.y -= height;
  return this;
}

TableLayout* TableLayout::row() {
  return row(_rowHeight);
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

} // namespace vigilante
