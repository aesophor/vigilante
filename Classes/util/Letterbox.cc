#include "Letterbox.h"

#include "Constants.h"

using cocos2d::Director;
using cocos2d::Scene;
using cocos2d::Size;
using cocos2d::GLView;

namespace vigilante {

void letterboxScale(Scene* scene) {
  // Perform letterbox scaling (solution provided by @makalele)
  // http://discuss.cocos2d-x.org/t/letterbox-scaling/19408/2
  GLView* glview = Director::getInstance()->getOpenGLView();
  Size size = glview->getFrameSize();
  float virtualWidth = vigilante::kVirtualWidth; // 600
  float virtualHeight = vigilante::kVirtualHeight; // 300
  float screenWidth = size.width;
  float screenHeight = size.height;
  float virtualRatio = virtualWidth / virtualHeight;
  float screenRatio = screenWidth / screenHeight;
  float scaleY = 1;

  if (screenRatio > virtualRatio) {
    // Left and right letterboxing
    scaleY = virtualRatio / screenRatio;
  } else if (screenRatio < virtualRatio) {
    // Up and bottom letterboxing
    //scaleX = screenRatio / virtualRatio;
    scaleY = screenRatio / virtualRatio;
  }

  scene->setScaleY(scaleY);
}

} // namespace vigilante
