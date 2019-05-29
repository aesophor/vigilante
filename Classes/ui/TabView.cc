#include "TabView.h"

#include "AssetManager.h"

using std::vector;
using std::string;
using std::unique_ptr;
using cocos2d::Vec2;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

TabView::TabView(const string& regularImg, const string& highlightedImg)
    : _layout(Layout::create()),      
      _current(),
      _nextTabPos(),
      _regularImg(regularImg),
      _highlightedImg(highlightedImg) {
  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 0});
}

void TabView::addTab(const string& text) {
  _tabs.push_back(unique_ptr<Tab>(new Tab(this, text)));

  const auto& tabSize = _tabs.back()->_background->getContentSize();
  _nextTabPos.x += ((_nextTabPos.x == 0) ? tabSize.width / 2 : tabSize.width) + 1;
  _nextTabPos.y += (_nextTabPos.y == 0) ? tabSize.height / 2 : 0;
  _tabs.back()->_background->setPosition(_nextTabPos);
  _tabs.back()->_label->setPosition(_nextTabPos);

  _layout->addChild(_tabs.back()->_background);
  _layout->addChild(_tabs.back()->_label);
}

void TabView::selectTab(int index) {
  if (index >= 0 && index < (int) _tabs.size()) {
    _tabs[_current]->setIsSelected(false);
    _tabs[index]->setIsSelected(true);
    _current = index;
  }
}

void TabView::selectPrev() {
  int prev = _current - 1;
  if (prev < 0) {
    prev = _tabs.size() - 1;
  }
  selectTab(prev);
}

void TabView::selectNext() {
  selectTab((_current + 1) % _tabs.size());
}


TabView::Tab* TabView::getSelectedTab() const {
  return _tabs[_current].get();
}

Layout* TabView::getLayout() const {
  return _layout;
}


TabView::Tab::Tab(TabView* parent, const string& text)
    : _parent(parent),
      _background(ImageView::create(parent->_regularImg)), 
      _label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      _isSelected(),
      _index(parent->_tabs.size()) {
  _label->getFontAtlas()->setAliasTexParameters();
}

bool TabView::Tab::isSelected() const {
  return _isSelected;
}

void TabView::Tab::setIsSelected(bool isSelected) {
  if (isSelected) {
    _background->loadTexture(_parent->_highlightedImg);
  } else {
    _background->loadTexture(_parent->_regularImg);
  }
  _isSelected = isSelected;
}

int TabView::Tab::getIndex() const {
  return _index;
}

} // namespace vigilante
