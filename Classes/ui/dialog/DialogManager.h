#ifndef VIGILANTE_DIALOG_MANAGER_H_
#define VIGILANTE_DIALOG_MANAGER_H_

#include <string>
#include <queue>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UIImageView.h"

#include "Controllable.h"

namespace vigilante {

class DialogManager : public Controllable {
 public:
  static DialogManager* getInstance();
  virtual ~DialogManager() = default;

  void update(float delta);
  virtual void handleInput() override; // Controllable

  void add(const std::string& speakerName, const std::string& content);
  void showNextDialog();
  cocos2d::Layer* getLayer() const;

 private:
  struct Dialog {
    Dialog(const std::string& speakerName, const std::string& content);
    std::string speakerName;
    std::string content;
  };

  static DialogManager* _instance;
  DialogManager();

  static const float _kShowCharInterval;

  cocos2d::Layer* _layer;
  cocos2d::Label* _label;
  cocos2d::ui::ImageView* _nextDialogIcon;

  std::queue<DialogManager::Dialog> _dialogQueue;
  DialogManager::Dialog _currentDialog;
  float _timer;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOG_MANAGER_H_
