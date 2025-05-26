// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "AppDelegate.h"

#ifdef __linux__
extern "C" {
#include <unistd.h>
}
#endif

#include <string>

#include <audio/AudioEngine.h>

#include "Assets.h"
#include "Audio.h"
#include "Constants.h"
#include "scene/SceneManager.h"
#include "scene/MainMenuScene.h"

using namespace std;
using requiem::kVirtualWidth;
using requiem::kVirtualHeight;
USING_NS_AX;

namespace {

auto designResolutionSize = ax::Size{1280, 800};
auto smallResolutionSize = ax::Size{480, 320};
auto mediumResolutionSize = ax::Size{1024, 768};
auto largeResolutionSize = ax::Size{2048, 1536};

}  // namespace

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
  return 0;  // flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
  // Initialize director
  Director* director = Director::getInstance();
  GLView* glview = director->getGLView();

  if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    glview = GLViewImpl::createWithRect("Requiem", ax::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
    glview = GLViewImpl::create("Requiem");
#endif
    director->setGLView(glview);
  }

  glview->setDesignResolutionSize(kVirtualWidth, kVirtualHeight, ResolutionPolicy::SHOW_ALL);

  // set FPS. the default value is 1.0/60 if you don't call this
  director->setAnimationInterval(1.0f / 60);

#ifdef __linux__
  chdir("Resources");
#endif

  requiem::assets::loadSpritesheets();
  requiem::SceneManager::the().runWithScene(requiem::MainMenuScene::create());

  return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

  AudioEngine::pauseAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

  AudioEngine::resumeAll();
}
