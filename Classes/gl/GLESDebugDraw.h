/*
 * Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
 * Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>
 *
 * iPhone port by Simon Oliver - http://www.simonoliver.com - http://www.handcircus.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
#ifndef VIGILANTE_RENDER_H_
#define VIGILANTE_RENDER_H_

#include <cocos2d.h>
#include <Box2D/Box2D.h>

struct b2AABB;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class GLESDebugDraw : public b2Draw {
 public:
  GLESDebugDraw();
  GLESDebugDraw(float32 ratio);
  virtual ~GLESDebugDraw() = default;

  void DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color);
  void DrawSolidPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color);
  void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
  void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
  void DrawTransform(const b2Transform& xf);
  void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
  void DrawString(int x, int y, const char* string, ...); 
  void DrawAABB(b2AABB* aabb, const b2Color& color);

 private:
  void initShader();

  float32 _ratio;
  cocos2d::GLProgram* _shaderProgram;
  GLint _colorLocation;
};

#endif // VIGILANTE_RENDERER_H_
