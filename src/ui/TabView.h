// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TABVIEW_H_
#define VIGILANTE_TABVIEW_H_

#include <memory>
#include <string>
#include <vector>

#include <2d/CCLabel.h>
#include <cocos2d.h>
#include <ui/UIImageView.h>
#include <ui/UILayout.h>

namespace vigilante {

class TabView {
 public:
  TabView(const std::string& regularBg, const std::string& highlightedBg);
  virtual ~TabView() = default;

  class Tab {
   public:
    friend class TabView;
    Tab(TabView* parent, const std::string& text);
    virtual ~Tab() = default;

    bool isSelected() const;
    void setIsSelected(bool isSelected);
    int getIndex() const;

   private:
    TabView* _parent;
    cocos2d::ui::ImageView* _background;
    cocos2d::Label* _label;
    bool _isSelected;
    int _index;
  };

  virtual void addTab(const std::string& text);
  virtual void selectTab(int index);
  virtual void selectPrev();
  virtual void selectNext();

  TabView::Tab* getSelectedTab() const;
  cocos2d::ui::Layout* getLayout() const;

 protected:
  cocos2d::ui::Layout* _layout;
  std::vector<std::unique_ptr<TabView::Tab>> _tabs;
  int _current;
  cocos2d::Vec2 _nextTabPos;

  std::string _regularBg;
  std::string _highlightedBg;
};

}  // namespace vigilante

#endif  // VIGILANTE_TABVIEW_H_
