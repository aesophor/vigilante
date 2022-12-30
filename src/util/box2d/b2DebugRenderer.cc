// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
/*
 * Created by Stefan Nguyen on Oct 8, 2012.
 * Copyright (c) 2013 Stefan Nguyen
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
#include "b2DebugRenderer.h"

USING_NS_CC;

b2DebugRenderer* b2DebugRenderer::create(b2World* world) {
  b2DebugRenderer *pRet = new b2DebugRenderer(world);
  if (pRet && pRet->init()) {
    pRet->autorelease();
    return pRet;
  } else {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }
}

b2DebugRenderer::b2DebugRenderer(b2World* world) : _world(world) {}

bool b2DebugRenderer::init() {
  mB2DebugDraw = new GLESDebugDraw(100);
  _world->SetDebugDraw(mB2DebugDraw);
  uint32 flags = 0;
  flags += b2Draw::e_shapeBit;
  flags += b2Draw::e_jointBit;
  //flags += b2Draw::e_aabbBit;
  flags += b2Draw::e_pairBit;
  flags += b2Draw::e_centerOfMassBit;
  mB2DebugDraw->SetFlags(flags);
  
  return true;
}


void b2DebugRenderer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags) {
  //Sprite::draw(renderer, transform, flags);
  _customCmd.init(_globalZOrder, transform, flags);
  _customCmd.func = CC_CALLBACK_0(b2DebugRenderer::onDraw, this, transform, flags);
  renderer->addCommand(&_customCmd);
}

void b2DebugRenderer::onDraw(const Mat4& transform, uint32_t flags) {
  Director* director = Director::getInstance();
  CCASSERT(director, "Director is null when seting matrix stack");
  director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

  cocos2d::GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);

  _world->DrawDebugData();

  CHECK_GL_ERROR_DEBUG();

  director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
