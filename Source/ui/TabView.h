// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_TABVIEW_H_
#define VIGILANTE_UI_TABVIEW_H_

#include <vector>
#include <string>
#include <memory>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

namespace vigilante {

class TabView {
 public:
  TabView(const std::string& regularBg, const std::string& highlightedBg);
  virtual ~TabView() = default;

  class Tab {
    friend class TabView;

   public:
    Tab(TabView* parent, const std::string& text);
    virtual ~Tab() = default;

    inline int getIndex() const { return _index; }
    inline bool isSelected() const { return _isSelected; }
    void setIsSelected(bool isSelected);

   private:
    TabView* _parent;
    ax::ui::ImageView* _background;
    ax::Label* _label;
    int _index;
    bool _isSelected;
  };

  virtual void addTab(const std::string& text);
  virtual void selectTab(int index);
  virtual void selectPrev();
  virtual void selectNext();

  TabView::Tab* getSelectedTab() const;
  inline ax::ui::Layout* getLayout() const { return _layout; }

 protected:
  ax::ui::Layout* _layout;
  std::vector<std::unique_ptr<TabView::Tab>> _tabs;
  int _current{};
  ax::Vec2 _nextTabPos{0, 0};

  std::string _regularBg;
  std::string _highlightedBg;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_TABVIEW_H_
