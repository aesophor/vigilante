#ifndef VIGILANTE_HUD_H_
#define VIGILANTE_HUD_H_

#include "cocos2d.h"


namespace vigilante {

class Hud : public cocos2d::Layer {
 public:
  CREATE_FUNC(Hud);
  virtual ~Hud();

  virtual bool init() override;
  virtual void update(float delta) override;

 private:
};

} // namespace vigilante

#endif // VIGILANTE_HUD_H_
