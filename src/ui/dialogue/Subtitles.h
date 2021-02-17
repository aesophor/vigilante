// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
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
  Subtitles();
  virtual ~Subtitles() = default;

  void update(float delta);
  virtual void handleInput() override;  // Controllable

  void addSubtitle(const std::string& subtitle);
  void beginSubtitles();
  void endSubtitles();
  void showNextSubtitle();

  cocos2d::Layer* getLayer() const;

 private:
  struct Subtitle {
    explicit Subtitle(const std::string& text);
    std::string text;
  };

  cocos2d::Layer* _layer;
  cocos2d::Label* _label;
  cocos2d::ui::ImageView* _nextSubtitleIcon;
  cocos2d::ui::ImageView* _upperLetterbox;
  cocos2d::ui::ImageView* _lowerLetterbox;

  std::queue<Subtitles::Subtitle> _subtitleQueue;
  Subtitles::Subtitle _currentSubtitle;
  bool _isTransitioning;
  float _timer;
};

}  // namespace vigilante

#endif  // VIGILANTE_SUBTITLES_H_
