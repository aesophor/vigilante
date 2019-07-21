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

  void addSubtitle(const std::string& subtitle);
  void beginSubtitles();
  void endSubtitles();
  cocos2d::Layer* getLayer() const;

 private:
  struct Subtitle {
    Subtitle(const std::string& text);
    std::string text;
  };

  static Subtitles* _instance;
  Subtitles();

  void showNextSubtitle();

  cocos2d::Layer* _layer;
  cocos2d::Label* _label;
  cocos2d::ui::ImageView* _nextSubtitleIcon;
  cocos2d::ui::ImageView* _upperLetterbox;
  cocos2d::ui::ImageView* _lowerLetterbox;

  std::queue<Subtitles::Subtitle> _subtitleQueue;
  Subtitles::Subtitle _currentSubtitle;
  float _timer;
};

} // namespace vigilante

#endif // VIGILANTE_SUBTITLES_H_
