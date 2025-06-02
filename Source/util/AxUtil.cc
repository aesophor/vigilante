// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "AxUtil.h"

#include "util/Logger.h"

USING_NS_AX;

namespace requiem::ax_util {

void addChildWithParentCameraMask(Node* parent, Node* child) {
  child->setCameraMask(parent->getCameraMask());
  parent->addChild(child);
}

void addChildWithParentCameraMask(Node* parent, Node* child, const int localZOrder) {
  child->setCameraMask(parent->getCameraMask());
  parent->addChild(child, localZOrder);
}

bool setAliasTexParameters(ax::ui::ImageView* imageView) {
  auto imageRenderer = dynamic_cast<ax::Sprite*>(imageView->getVirtualRenderer());
  if (!imageRenderer) {
    VGLOG(LOG_ERR, "Failed to cast imageView->getVirtualRenderer() to sprite.");
    return false;
  }

  imageRenderer->getTexture()->setAliasTexParameters();
  return true;
}

}  // namespace requiem::ax_util
