#ifndef VIGILANTE_DUST_H_
#define VIGILANTE_DUST_H_

#include "cocos2d.h"

namespace vigilante {

class Dust {
 public:
  static void create(cocos2d::Layer* gameMapLayer, float x, float y);
 private:
  static cocos2d::Animation* _animation;
};

} // namespace vigilante

#endif // VIGILANTE_DUST_H_
