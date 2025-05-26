// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_LIST_VIEW_H_
#define REQUIEM_UI_LIST_VIEW_H_

#include <set>
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <functional>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

#include "Assets.h"
#include "Constants.h"
#include "ui/TableLayout.h"
#include "util/ds/SetVector.h"

namespace requiem {

class PauseMenu;

template <typename T>
class ListView {
  friend class ListViewItem;

 public:
  ListView(int visibleItemCount, float width, float height, float itemGapHeight,
           const std::string& regularBg=assets::kEmptyImage,
           const std::string& highlightedBg=assets::kEmptyImage,
           const std::string& font=assets::kRegularFont,
           const float fontSize=assets::kRegularFontSize);
  virtual ~ListView() = default;

  virtual void confirm() = 0;
  virtual void selectUp();
  virtual void selectDown();
  virtual void scrollUp();
  virtual void scrollDown();

  void showFrom(int index);  // show n ListViewItems starting from the specified index.

  template <template <typename...> class ContainerType>
  void setObjects(const ContainerType<T>& objects);

  void showScrollBar();
  void hideScrollBar();

  T getSelectedObject() const;
  ax::ui::Layout* getLayout() const { return _layout; }
  ax::Size getContentSize() const;

 protected:
  class ListViewItem {
   public:
    ListViewItem(ListView<T>* parent, float x, float y);
    virtual ~ListViewItem() = default;

    void setSelected(bool selected);
    void setVisible(bool visible);

    T getObject() const { return _object; }
    void setObject(T object);

    ax::ui::Layout* getLayout() const { return _layout; }
    ax::ui::ImageView* getBackground() const { return _background; }
    ax::ui::ImageView* getIcon() const { return _icon; }
    ax::Label* getLabel() const { return _label; }

   private:
    static inline constexpr int _kListViewIconSize = 16;

    ListView<T>* _parent;
    TableLayout* _layout;

    ax::ui::ImageView* _background;
    ax::ui::ImageView* _icon;
    ax::Label* _label;
    T _object{};
  };

  ax::ui::Layout* _layout;
  ax::ui::ImageView* _scrollBar;

  std::vector<std::unique_ptr<ListViewItem>> _listViewItems;
  std::deque<T> _objects;

  // called at the end of ListViewItem::setSelected()
  std::function<void (ListViewItem*, bool)> _setSelectedCallback;
  // called at the end of ListViewItem::setObject()
  std::function<void (ListViewItem*, T)> _setObjectCallback;

  int _visibleItemCount;
  float _width;
  float _height;  // determines the height of the scrollbar
  float _itemGapHeight;
  std::string _regularBg;
  std::string _highlightedBg;
  std::string _font;
  float _fontSize;

  int _firstVisibleIndex{};
  int _current{};
  bool _showScrollBar{true};
};

template <typename T>
ListView<T>::ListView(int visibleItemCount, float width, float height, float itemGapHeight,
                      const std::string& regularBg, const std::string& highlightedBg,
                      const std::string& font, const float fontSize)
    : _layout{ax::ui::Layout::create()},
      _scrollBar{ax::ui::ImageView::create(std::string{assets::kScrollBar})},
      _visibleItemCount{visibleItemCount},
      _width{width},
      _height{height},
      _itemGapHeight{itemGapHeight},
      _regularBg{regularBg},
      _highlightedBg{highlightedBg},
      _font{font},
      _fontSize{fontSize} {
  _scrollBar->setPosition({width, 0});
  _scrollBar->setAnchorPoint({0, 1});
  _scrollBar->setScaleY(height);
  _layout->addChild(_scrollBar);

  for (int i = 0; i < visibleItemCount; i++) {
    float x = 0;
    float y = itemGapHeight * (-i);
    _listViewItems.push_back(std::make_unique<ListViewItem>(this, x, y));
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

    if (index + i < (int) _objects.size()) {
      _listViewItems[i]->setVisible(true);
      T object = _objects[index + i];
      _listViewItems[i]->setObject(object);
    } else {
      _listViewItems[i]->setVisible(false);
    }
  }

  // Update scroll bar scale and positionY.
  if (_showScrollBar) {
    if (_objects.size() <= _visibleItemCount) {
      _scrollBar->setVisible(false);
    } else {
      _scrollBar->setScaleY(((float) _visibleItemCount / _objects.size()) * _height);
      _scrollBar->setPositionY(((float) -index / _objects.size()) * _height);
      _scrollBar->setVisible(true);
    }
  }
}

template <typename T>
template <template <typename...> class ContainerType>
void ListView<T>::setObjects(const ContainerType<T>& objects) {
  _objects = std::deque<T>(objects.begin(), objects.end());

  if (_current < 0) {
    _current = 0;
    _firstVisibleIndex = 0;
  } else if (_current >= _objects.size()) {
    _current = static_cast<int>(_objects.size() - 1);
    _firstVisibleIndex = std::max(0, _current - (_visibleItemCount - 1));
  }

  showFrom(_firstVisibleIndex);

  if (!_objects.empty()) {
    _listViewItems[_current - _firstVisibleIndex]->setSelected(true);
  }
}

template <typename T>
void ListView<T>::showScrollBar() {
  _showScrollBar = true;
  _scrollBar->setVisible(true);
}

template <typename T>
void ListView<T>::hideScrollBar() {
  _showScrollBar = false;
  _scrollBar->setVisible(false);
}

template <typename T>
T ListView<T>::getSelectedObject() const {
  return (_current < _objects.size()) ? _objects[_current] : nullptr;
}

template <typename T>
ax::Size ListView<T>::getContentSize() const {
  ax::Size retSize = {.0, .0};

  for (const auto& listViewItem : _listViewItems) {
    // The width of each listViewItem can be approximated by the following formula:
    // listViewItemWidth = std::max(iconWidth + labelWidth, backgroundWidth);
    auto iconSize = listViewItem->getIcon()->getContentSize();
    auto labelSize = listViewItem->getLabel()->getContentSize();
    auto backgroundSize = listViewItem->getBackground()->getContentSize();

    float listViewItemWidth = std::max(iconSize.width + labelSize.width, backgroundSize.width);
    float listViewItemHeight = std::max(iconSize.height, labelSize.height);

    retSize.width = std::max(retSize.width, listViewItemWidth);
    retSize.height += listViewItemHeight;
  }

  // Remember to add the gap (height) between listViewItems.
  retSize.height += _itemGapHeight * (_listViewItems.size() - 1);
  return retSize;
}

template <typename T>
ListView<T>::ListViewItem::ListViewItem(ListView<T>* parent, float x, float y)
    : _parent{parent},
      _layout{TableLayout::create(parent->_width)},
      _background{ax::ui::ImageView::create(parent->_regularBg)},
      _icon{ax::ui::ImageView::create(std::string{assets::kEmptyImage})},
      _label{ax::Label::createWithTTF("---", parent->_font, parent->_fontSize)} {
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

  if (_parent->_setSelectedCallback) {
    _parent->_setSelectedCallback(this, selected);
  }
}

template <typename T>
void ListView<T>::ListViewItem::setVisible(bool visible) {
  _layout->setVisible(visible);
}

template <typename T>
void ListView<T>::ListViewItem::setObject(T object) {
  _object = object;

  if (_parent->_setObjectCallback) {
    _parent->_setObjectCallback(this, object);
  }
}

}  // namespace requiem

#endif  // REQUIEM_UI_LIST_VIEW_H_
