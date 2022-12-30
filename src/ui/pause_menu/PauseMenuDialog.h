// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PAUSE_MENU_DIALOG_H_
#define VIGILANTE_PAUSE_MENU_DIALOG_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/PauseMenu.h"

namespace vigilante {

class PauseMenuDialog : public AbstractPane {
 public:
  explicit PauseMenuDialog(PauseMenu* pauseMenu);
  virtual ~PauseMenuDialog() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectLeft();
  void selectRight();
  void confirm();

  void reset();
  void setMessage(const std::string& message) const;
  void setOption(int index, bool visible, const std::string& text="", const std::function<void ()>& handler=[](){}) const;
  void show();

 private:
  class Option {
   public:
    explicit Option(const std::string& text, const std::function<void ()>& handler=[](){});
    virtual ~Option() = default;

    float getWidth() const;
    inline void setSelected(bool selected) const { _icon->setVisible(selected); }
    inline bool isVisible() const { return _layout->isVisible(); }
    inline void setVisible(bool visible) const { _layout->setVisible(visible); }
    inline cocos2d::ui::Layout* getLayout() const { return _layout; }

    inline std::string getText() const { return _label->getString(); }
    inline void setText(const std::string& text) const { _label->setString(text); }

    inline const std::function<void ()>& getHandler() const { return _handler; }
    inline void setHandler(const std::function<void ()>& handler) { _handler = handler; }

   private:
    static inline constexpr int _kIconLabelGap = 1;

    cocos2d::ui::Layout* _layout;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    std::function<void ()> _handler;
  };

  // Reserved for future use. Currently there's only three options at most.
  void addOption(const std::string& text, const std::function<void ()>& handler=[](){});
  void clearOptions();

  cocos2d::Label* _message;
  std::vector<std::unique_ptr<PauseMenuDialog::Option>> _options;
  int _current;
};

}  // namespace vigilante

#endif  // VIGILANTE_PAUSE_MENU_DIALOG_H_
