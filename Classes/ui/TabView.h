#ifndef VIGILANTE_TABVIEW_H_
#define VIGILANTE_TABVIEW_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

namespace vigilante {

class TabView {
 public:
  TabView(const std::string& regularImg, const std::string& highlightedImg);
  virtual ~TabView() = default;

  virtual void addTab(const std::string& text);
  virtual void selectTab(int index);
  virtual void selectPrev();
  virtual void selectNext();
  cocos2d::ui::Layout* getLayout() const;

 protected:
  class Tab {
   public:
    friend class TabView;
    Tab(TabView* parent, const std::string& text);
    virtual ~Tab() = default;

    bool isSelected() const;
    void setIsSelected(bool isSelected);

   private:
    TabView* _parent;
    cocos2d::ui::ImageView* _background;
    cocos2d::Label* _label;
    bool _isSelected;
  };

  cocos2d::ui::Layout* _layout;
  std::vector<std::unique_ptr<TabView::Tab>> _tabs;
  int _current;
  cocos2d::Vec2 _nextTabPos;

  std::string _regularImg;
  std::string _highlightedImg;
};

} // namespace vigilante

#endif // VIGILANTE_TABVIEW_H_
