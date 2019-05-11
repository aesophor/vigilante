#include "Hud.h"

/* originally written in MainGameScene.cc
 * This is the code to render hud on the top left of the screen.
 
  auto oc = Camera::createOrthographic(winSize.width, winSize.height, 1, 1000);
  oc->setDepth(2);
  oc->setCameraFlag(CameraFlag::USER1);

  const Vec3& eyePosOld = getDefaultCamera()->getPosition3D();
  Vec3 eyePos = {eyePosOld.x, eyePosOld.y, eyePosOld.z};
  oc->setPosition3D(eyePos);
  oc->lookAt(eyePos);
  oc->setPosition(0, 0);
  addChild(oc);


  auto hudLayer = Layer::create();

  auto sprite = Sprite::create("lol.png");
  //sprite->setScaleX(3.0f);
  sprite->getTexture()->setAliasTexParameters();
  //sprite->setPosition(0, 0);

  hudLayer->addChild(sprite);
  hudLayer->setCameraMask((uint16_t) CameraFlag::USER1);
  hudLayer->setPosition(80, winSize.height - sprite->getContentSize().height / 2);
  addChild(hudLayer);
  */
