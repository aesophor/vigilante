// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_AX_UTIL_H_
#define VIGILANTE_UTIL_AX_UTIL_H_

#include <axmol.h>

namespace vigilante::ax_util {

void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child);
void addChildWithParentCameraMask(ax::Node* parent, ax::Node* child, const int localZOrder);

}  // namespace vigilante::ax_util

#endif  // VIGILANTE_UTIL_AX_UTIL_H_
