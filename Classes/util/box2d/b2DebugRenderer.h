/*
 * Created by Stefan Nguyen on Oct 8, 2012.
 * Copyright (c) 2013 Stefan Nguyen
 * Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VIGILANTE_B2_DEBUG_RENDERER_H_
#define VIGILANTE_B2_DEBUG_RENDERER_H_

#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "gl/GLESDebugDraw.h"

class b2DebugRenderer : public cocos2d::Sprite {
 public:
  explicit b2DebugRenderer(b2World* world);
  static b2DebugRenderer* create(b2World* world);

  bool init() override;
  void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
  void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

 private:
  b2World* _world;
  GLESDebugDraw* mB2DebugDraw;
  cocos2d::CustomCommand _customCmd;
};

#endif // VIGILANTE_B2_DEBUG_RENDERER_H_
