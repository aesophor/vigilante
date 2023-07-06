// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SUBTITLES_H_
#define VIGILANTE_SUBTITLES_H_

#include <string>
#include <queue>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>
#include "Controllable.h"

namespace vigilante {

class Subtitles : public Controllable {
 public:
  Subtitles();
  virtual ~Subtitles() override = default;

  void update(float delta);
  virtual void handleInput() override;  // Controllable

  void addSubtitle(const std::string& subtitle);
  void beginSubtitles();
  void endSubtitles();
  void showNextSubtitle();

  inline ax::Layer* getLayer() const { return _layer; }

 private:
  struct Subtitle {
    explicit Subtitle(const std::string& text) : text(text) {}
    std::string text;
  };

  ax::Layer* _layer;
  ax::Label* _label;
  ax::ui::ImageView* _nextSubtitleIcon;
  ax::ui::ImageView* _upperLetterbox;
  ax::ui::ImageView* _lowerLetterbox;

  std::queue<Subtitles::Subtitle> _subtitleQueue;
  Subtitles::Subtitle _currentSubtitle;
  bool _isTransitioning;
  float _timer;
};

}  // namespace vigilante

#endif  // VIGILANTE_SUBTITLES_H_
