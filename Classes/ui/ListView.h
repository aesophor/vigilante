// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_LIST_VIEW_H_
#define VIGILANTE_LIST_VIEW_H_

#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <functional>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>
#include "AssetManager.h"
#include "Constants.h"
#include "ui/TableLayout.h"

namespace vigilante {

class PauseMenu;

template <typename T>
class ListView {
 public:
  ListView(uint8_t visibleItemCount, uint32_t width,
           const std::string& regularBg=asset_manager::kEmptyImage,
           const std::string& highlightedBg=asset_manager::kEmptyImage);
  virtual ~ListView() = default;

  virtual void confirm() = 0;
  virtual void selectUp();
  virtual void selectDown();
  virtual void scrollUp();
  virtual void scrollDown();
  virtual void showFrom(int index); // show n ListViewItems starting from the specified index.

  T getSelectedObject() const;
  cocos2d::ui::Layout* getLayout() const;

 protected:
  friend class ListViewItem;

  class ListViewItem {
   public:
    ListViewItem(ListView<T>* parent, float x, float y);
    virtual ~ListViewItem() = default;

    void setSelected(bool selected);
    void setVisible(bool visible);

    T getObject() const;
    void setObject(T object);

    cocos2d::ui::Layout* getLayout() const;
    cocos2d::ui::ImageView* getIcon() const;
    cocos2d::Label* getLabel() const;

   private:
    static const int _kListViewIconSize;

    ListView<T>* _parent;
    TableLayout* _layout;

    cocos2d::ui::ImageView* _background;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    T _object;
  };

  cocos2d::ui::Layout* _layout;
 
  std::vector<std::unique_ptr<ListViewItem>> _listViewItems;
  std::deque<T> _objects;
  std::function<void (ListViewItem*, T)> _onSelect; // called at the end of ListViewItem::setObject()

  uint8_t _visibleItemCount;
  uint32_t _width;
  std::string _regularBg;
  std::string _highlightedBg;

  int _firstVisibleIndex;
  int _current;
};



template <typename T>
ListView<T>::ListView(uint8_t visibleItemCount, uint32_t width,
                      const std::string& regularBg, const std::string& highlightedBg)
    : _layout(cocos2d::ui::Layout::create()),
      _visibleItemCount(visibleItemCount),
      _width(width),
      _regularBg(regularBg),
      _highlightedBg(highlightedBg),
      _firstVisibleIndex(),
      _current() {
  for (int i = 0; i < visibleItemCount; i++) {
    float x = 5;
    float y = -5.0f - i * 25;
    _listViewItems.push_back(std::unique_ptr<ListViewItem>(new ListViewItem(this, x, y)));
    _listViewItems.back()->setVisible(false);
    _layout->addChild(_listViewItems[i]->getLayout());
  }
}


template <typename T>
void ListView<T>::selectUp() {
  // If currently selected item is the first visible item, and we still can scroll up,
  // then update the selected item.
  if (_current <= 0) {
    return;
  }

  if (_current == _firstVisibleIndex) {
    scrollUp();
  }
  _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
  _listViewItems[--_current - _firstVisibleIndex]->setSelected(true);
}

template <typename T>
void ListView<T>::selectDown() {
  if (_current >= (int) _objects.size() - 1) {
    return;
  }

  // If currently selected item is the last visible item, and we still can scroll down,
  // then update the selected item.
  if (_current == _firstVisibleIndex + _visibleItemCount - 1) {
    scrollDown();
  }
  _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
  _listViewItems[++_current - _firstVisibleIndex]->setSelected(true);
}

template <typename T>
void ListView<T>::scrollUp() {
  if ((int) _objects.size() <= _visibleItemCount || _firstVisibleIndex == 0) {
    return;
  }
  showFrom(--_firstVisibleIndex);
}

template <typename T>
void ListView<T>::scrollDown() {
  if ((int) _objects.size() <= _visibleItemCount ||
      (int) _objects.size() <= _firstVisibleIndex + _visibleItemCount) {
    return;
  }
  showFrom(++_firstVisibleIndex);
}


template <typename T>
void ListView<T>::showFrom(int index) {
  // Show n items starting from the given index.
  for (int i = 0; i < _visibleItemCount; i++) {
    _listViewItems[i]->setSelected(false);

    if (index < (int) _objects.size()) {
      _listViewItems[i]->setVisible(true);
      T object = _objects[index];
      _listViewItems[i]->setObject(object);
      index++;
    } else {
      _listViewItems[i]->setVisible(false);
    }
  }
}

template <typename T>
T ListView<T>::getSelectedObject() const {
  if (!_objects.empty() && _listViewItems[_current]) {
    return _listViewItems[_current]->getObject();
  }
  return nullptr;
}

template <typename T>
cocos2d::ui::Layout* ListView<T>::getLayout() const {
  return _layout;
}


template <typename T>
const int ListView<T>::ListViewItem::_kListViewIconSize = 16;

template <typename T>
ListView<T>::ListViewItem::ListViewItem(ListView<T>* parent, float x, float y)
    : _parent(parent),
      _layout(TableLayout::create(parent->_width)),
      _background(cocos2d::ui::ImageView::create(parent->_regularBg)),
      _icon(cocos2d::ui::ImageView::create(asset_manager::kEmptyImage)),
      _label(cocos2d::Label::createWithTTF("---", asset_manager::kRegularFont, asset_manager::kRegularFontSize)),
      _object() {
  _icon->setScale((float) _kListViewIconSize / kIconSize);

  _background->setAnchorPoint({0, 1});
  _layout->setPosition({x, y});
  _layout->addChild(_background);
  _layout->row(1);

  _layout->addChild(_icon);
  _layout->align(TableLayout::Alignment::LEFT)->padLeft(5)->spaceX(5);

  _label->setAnchorPoint({0, 1});
  _label->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_label);
  _layout->padTop(1);
}

template <typename T>
void ListView<T>::ListViewItem::setSelected(bool selected) {
  _background->loadTexture((selected) ? _parent->_highlightedBg : _parent->_regularBg);
}

template <typename T>
void ListView<T>::ListViewItem::setVisible(bool visible) {
  _layout->setVisible(visible);
}

template <typename T>
T ListView<T>::ListViewItem::getObject() const {
  return _object;
}

template <typename T>
void ListView<T>::ListViewItem::setObject(T object) {
  _object = object;

  // Provide extra logic for setting _icon and _label.
  // See the ctor in ui/pause_menu/inventory/ItemListView.cc for example.
  _parent->_onSelect(this, object);
}

template <typename T>
cocos2d::ui::Layout* ListView<T>::ListViewItem::getLayout() const {
  return _layout;
}

template <typename T>
cocos2d::ui::ImageView* ListView<T>::ListViewItem::getIcon() const {
  return _icon;
}

template <typename T>
cocos2d::Label* ListView<T>::ListViewItem::getLabel() const {
  return _label;
}

} // namespace vigilante

#endif // VIGILANTE_LIST_VIEW_H_
