// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SUBTITLES_H_
#define VIGILANTE_SUBTITLES_H_

#include <string>
#include <queue>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>
#include "Controllable.h"

namespace vigilante {

class Subtitles : public Controllable {
 public:
  static Subtitles* getInstance();
  virtual ~Subtitles() = default;

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

  static Subtitles* _instance;
  Subtitles();

  void showNextDialog();

  static const float _kShowCharInterval;
  static const int _kLetterboxHeight;

  cocos2d::Layer* _layer;
  cocos2d::Label* _label;
  cocos2d::ui::ImageView* _nextDialogIcon;
  cocos2d::ui::ImageView* _upperLetterbox;
  cocos2d::ui::ImageView* _lowerLetterbox;

  std::queue<Subtitles::Dialog> _dialogQueue;
  Subtitles::Dialog _currentDialog;
  float _timer;
};

} // namespace vigilante

#endif // VIGILANTE_SUBTITLES_H_
