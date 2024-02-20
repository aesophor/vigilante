// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "AxUtil.h"

USING_NS_AX;

namespace vigilante::ax_util {

void addChildWithParentCameraMask(Node* parent, Node* child) {
  child->setCameraMask(parent->getCameraMask());
  parent->addChild(child);
}

void addChildWithParentCameraMask(Node* parent, Node* child, const int localZOrder) {
  child->setCameraMask(parent->getCameraMask());
  parent->addChild(child, localZOrder);
}

}  // namespace vigilante::ax_util
