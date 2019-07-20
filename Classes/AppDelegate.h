// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef APP_DELEGATE_H_
#define APP_DELEGATE_H_

#include <cocos2d.h>

// The cocos2d Application.
// Private inheritance here hides part of interface from Director.
class AppDelegate : private cocos2d::Application {
 public:
  AppDelegate();
  virtual ~AppDelegate();

  virtual void initGLContextAttrs();

  // Implement Director and Scene init here.
  virtual bool applicationDidFinishLaunching();

  // Called when the application is moved to the background.
  virtual void applicationDidEnterBackground();

  // Called when the application reenters the foreground.
  virtual void applicationWillEnterForeground();
};

#endif // APP_DELEGATE_H_
