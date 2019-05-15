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
#include "PauseMenu.h"

namespace vigilante {

class PauseMenuDialog : public AbstractPane {
 public:
  PauseMenuDialog(PauseMenu* pauseMenu);
  virtual ~PauseMenuDialog() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectLeft();
  void selectRight();
  void confirm();

  void reset() const;
  void setMessage(const std::string& message) const;
  void setOption(int index, bool visible, const std::string& text="", const std::function<void ()>& handler=[=](){}) const;
  void show();

 private:
  class Option {
   public:
    Option(const std::string& text, const std::function<void ()>& handler=[=](){});
    virtual ~Option() = default;

    float getWidth() const;
    void setSelected(bool selected) const;
    bool isVisible() const;
    void setVisible(bool visible) const;
    cocos2d::ui::Layout* getLayout() const;

    std::string getText() const;
    void setText(const std::string& text) const;

    const std::function<void ()>& getHandler() const;
    void setHandler(const std::function<void ()>& handler);

   private:
    cocos2d::ui::Layout* _layout;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    std::function<void ()> _handler;
  };

  // Reserved for future use. Currently there's only three options at most.
  void addOption(const std::string& text, const std::function<void ()>& handler=[=](){});
  void clearOptions();

  PauseMenu* _pauseMenu;

  cocos2d::Label* _message;
  std::vector<std::unique_ptr<PauseMenuDialog::Option>> _options;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_PAUSE_MENU_DIALOG_H_
