#include "NotificationManager.h"

#include "AssetManager.h"

using std::string;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::MoveBy;
using cocos2d::FadeOut;
using cocos2d::CallFunc;
using cocos2d::Sequence;
using cocos2d::CameraFlag;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

NotificationManager* NotificationManager::_instance = nullptr;

const float NotificationManager::kStartingX = 10.0f;
const float NotificationManager::kStartingY = 10.0f;

const float NotificationManager::kDeltaX = 0.0f;
const float NotificationManager::kDeltaY = 10.0f;

const float NotificationManager::kMoveUpDuration = .2f;
const float NotificationManager::kFadeDuration = 1.0f;

const int NotificationManager::kDefaultMaxNotificationCount = 10;

NotificationManager* NotificationManager::getInstance() {
  if (!_instance) {
    _instance = new NotificationManager();
  }
  return _instance;
}

NotificationManager::NotificationManager()
    : _layer(Layer::create()),
      _maxNotificationCount(kDefaultMaxNotificationCount) {}


void NotificationManager::update(float delta) {
  for (auto& notification : _notificationQueue) {
    notification.timer += delta;

    if (notification.timer >= notification.lifetime) {
      notification.label->runAction(Sequence::createWithTwoActions(
        FadeOut::create(kFadeDuration),
        CallFunc::create([=]() {
          // After the label fully fades out, remove the label from _layer.
          _layer->removeChild(notification.label);
        })
      ));
      // Also remove the notification object from _notificationQueue.
      auto& q = _notificationQueue;
      q.erase(std::remove(q.begin(), q.end(), notification), q.end());
    }
  }
}

void NotificationManager::show(const string& message) {
  // If the number of notifications being displayed has surpassed kMaxNotificationCount,
  // then remove the earliest notification.
  if (_notificationQueue.size() > kDefaultMaxNotificationCount) {
    _layer->removeChild(_notificationQueue.front().label);
    _notificationQueue.pop_front();
  }

  // Move previous notifications up.
  for (auto notification : _notificationQueue) {
    notification.label->runAction(MoveBy::create(kMoveUpDuration, {kDeltaX, kDeltaY}));
  }

  // Display the new notification.
  Notification notification(message, 5.0f);
  notification.label->setPosition(kStartingX, kStartingY);
  notification.label->runAction(MoveBy::create(kMoveUpDuration, {kDeltaX, kDeltaY}));
  _notificationQueue.push_back(notification);
  _layer->addChild(notification.label);
}

Layer* NotificationManager::getLayer() const {
  return _layer;
}


NotificationManager::Notification::Notification(const string& text, float lifetime)
    : label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      lifetime(lifetime),
      timer() {
  label->setAnchorPoint({0, 1});
  label->getFontAtlas()->setAliasTexParameters();
  label->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
}

bool NotificationManager::Notification::operator== (const Notification& other) {
  return this->label == other.label;
}

} // namespace vigilante
