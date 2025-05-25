// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "TimedLabelService.h"

#include "Assets.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

TimedLabelService::TimedLabelService(const float startingX, const float startingY,
                                     const uint8_t maxLabelCount, const uint8_t labelLifetime,
                                     const TimedLabelService::TimedLabel::Alignment alignment)
    : _layer{Layer::create()},
      _kStartingX{startingX},
      _kStartingY{startingY},
      _kMaxLabelCount{maxLabelCount},
      _kLabelLifetime{labelLifetime},
      _kAlignment{alignment} {}

void TimedLabelService::update(const float delta) {
  for (auto& notification : _labelQueue) {
    notification.timer += delta;

    if (notification.timer >= notification.lifetime) {
      notification.label->runAction(Sequence::createWithTwoActions(
        FadeOut::create(_kFadeDuration),
        CallFunc::create([this, notification]() {
          // After the label fully fades out, remove the label from _layer.
          _layer->removeChild(notification.label);
        })
      ));
      // Also remove the notification object from _labelQueue.
      auto& q = _labelQueue;
      q.erase(std::remove(q.begin(), q.end(), notification), q.end());
    }
  }
}

void TimedLabelService::show(const string& message) {
  // If the number of notifications being displayed has surpassed _kMaxLabelCount,
  // then remove the earliest notification.
  if ((int) _labelQueue.size() > _kMaxLabelCount) {
    _layer->removeChild(_labelQueue.front().label);
    _labelQueue.pop_front();
  }

  // Move previous notifications up.
  for (auto label : _labelQueue) {
    label.label->runAction(MoveBy::create(_kMoveUpDuration, {_kDeltaX, _kDeltaY}));
  }

  // Display the new notification.
  // Note that ax::Layer::setCameraMask() can only apply the given mask to
  // the children that are in the _layer at that moment. If we create a label/sprite
  // afterwards, the mask is the default one, even if we add it as a child of that layer.
  TimedLabel timedLabel(message, _kLabelLifetime, _kAlignment);
  timedLabel.label->setPosition(_kStartingX, _kStartingY);
  timedLabel.label->runAction(MoveBy::create(_kMoveUpDuration, {_kDeltaX, _kDeltaY}));
  _labelQueue.push_back(timedLabel);

  _layer->addChild(timedLabel.label);
  _layer->setCameraMask(_layer->getCameraMask());
}

TimedLabelService::TimedLabel::TimedLabel(const string& text, const float lifetime, const TimedLabel::Alignment alignment)
    : label{Label::createWithTTF(text, string{kRegularFont}, kRegularFontSize)},
      lifetime{lifetime} {
  label->setAnchorPoint(alignment);
  label->getFontAtlas()->setAliasTexParameters();
}

}  // namespace vigilante
