#include "AppDelegate.h"

#include "scene/MainGameScene.h"

#define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 800);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
  AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
  // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
  GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
  GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages() {
  return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
  // Initialize director
  Director* director = Director::getInstance();
  GLView* glview = director->getOpenGLView();

  if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    glview = GLViewImpl::createWithRect("Vigilante", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
    glview = GLViewImpl::create("Vigilante");
#endif
    director->setOpenGLView(glview);
  }

  // set FPS. the default value is 1.0/60 if you don't call this
  director->setAnimationInterval(1.0f / 60);

  // Set the design resolution
  //glview->setDesignResolutionSize(1280, 800, ResolutionPolicy::EXACT_FIT);
  glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
  auto frameSize = glview->getFrameSize();

  if (frameSize.height > mediumResolutionSize.height) {        
    // if the frame's height is larger than the height of medium size.
    director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
  } else if (frameSize.height > smallResolutionSize.height) {        
    // if the frame's height is larger than the height of small size.
    director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
  } else {        
    // if the frame's height is smaller than the height of medium size.
    director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
  }

  register_all_packages();

  // create a scene. it's an autorelease object
  Scene* scene = MainGameScene::create();

  // run
  director->runWithScene(scene);

  return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
  SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
  SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
