#include "AppDelegate.h"

#include <string>

#include "GameAssetManager.h"
#include "scene/MainMenuScene.h"
#include "scene/MainGameScene.h"
#include "util/Constants.h"

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

using std::string;
using vigilante::kVirtualWidth;
using vigilante::kVirtualHeight;

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

  glview->setDesignResolutionSize(kVirtualWidth, kVirtualHeight, ResolutionPolicy::SHOW_ALL);

  // set FPS. the default value is 1.0/60 if you don't call this
  director->setAnimationInterval(1.0f / 60);

  // Load resources
  cocos2d::log("[Dust] loading textures");
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  frameCache->addSpriteFramesWithFile(vigilante::asset_manager::kPlayerTextureResDir + "/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/character/vlad/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/character/slime/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/character/demon/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/character/skeleton/spritesheet.plist");
  frameCache->addSpriteFramesWithFile(vigilante::asset_manager::kRustyAxeTextureResDir + "/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/item/iron_broadsword/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/item/royal_cape/spritesheet.plist");
  frameCache->addSpriteFramesWithFile("Texture/spell/ice_spike/spritesheet.plist");
  frameCache->addSpriteFramesWithFile(vigilante::asset_manager::kDustTextureResDir + "/spritesheet.plist");

  // Create a scene (auto-release object).
  Scene* scene = vigilante::MainMenuScene::create();
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
