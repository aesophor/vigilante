#ifndef VIGILANTE_PAUSE_MENU_DIALOG_H_
#define VIGILANTE_PAUSE_MENU_DIALOG_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UIImageView.h"

#include "AbstractPane.h"

namespace vigilante {

class PauseMenuDialog : public AbstractPane {
 public:
  PauseMenuDialog();
  virtual ~PauseMenuDialog() = default;
  virtual void update() override;
  virtual void handleInput() override;

  void selectLeft();
  void selectRight();
  void confirm();

  void setMessage(const std::string& message) const;
  void addOption(const std::string& text, const std::function<void ()>& handler=[=](){});
  void clearOptions();

 private:
  class Option {
   public:
    Option(const std::string& text, const std::function<void ()>& handler=[=](){});
    virtual ~Option() = default;

    float getWidth() const;
    void setSelected(bool selected) const;
    cocos2d::ui::Layout* getLayout() const;
    const std::function<void ()>& getHandler() const;

   private:
    cocos2d::ui::Layout* _layout;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    std::function<void ()> _handler;
  };

  cocos2d::Label* _message;
  std::vector<std::unique_ptr<PauseMenuDialog::Option>> _options;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_PAUSE_MENU_DIALOG_H_
