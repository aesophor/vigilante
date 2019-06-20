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

  void addDialog(const std::string& dialog);
  void beginDialog();
  void endDialog();
  cocos2d::Layer* getLayer() const;

 private:
  struct Dialog {
    Dialog(const std::string& dialog);
    std::string dialog;
  };

  static DialogManager* _instance;
  DialogManager();

  void showNextDialog();

  static const float _kShowCharInterval;
  static const int _kLetterboxHeight;

  cocos2d::Layer* _layer;
  cocos2d::Label* _label;
  cocos2d::ui::ImageView* _nextDialogIcon;
  cocos2d::ui::ImageView* _upperLetterbox;
  cocos2d::ui::ImageView* _lowerLetterbox;

  std::queue<DialogManager::Dialog> _dialogQueue;
  DialogManager::Dialog _currentDialog;
  float _timer;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOG_MANAGER_H_
