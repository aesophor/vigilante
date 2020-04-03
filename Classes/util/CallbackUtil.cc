// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CallbackUtil.h"

using cocos2d::CallFunc;
using cocos2d::DelayTime;
using cocos2d::Scene;
using cocos2d::Sequence;
using std::function;

namespace {

Scene* scene = nullptr;

}  // namespace

namespace vigilante {

namespace callback_util {

void init(Scene* scene) {
  ::scene = scene;
}

void runAfter(const function<void()>& func, float delay) {
  auto delayAction = DelayTime::create(delay);
  auto callback = CallFunc::create(func);
  ::scene->runAction(Sequence::createWithTwoActions(delayAction, callback));
}

}  // namespace callback_util

}  // namespace vigilante
