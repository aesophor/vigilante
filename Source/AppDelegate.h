// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef APP_DELEGATE_H_
#define APP_DELEGATE_H_

#include <axmol.h>

// The axmol Application.
// Private inheritance here hides part of interface from Director.
class AppDelegate : private ax::Application {
 public:
  AppDelegate() = default;
  virtual ~AppDelegate();

  virtual void initGLContextAttrs() override;

  // Implement Director and Scene init here.
  virtual bool applicationDidFinishLaunching() override;

  // Called when the application is moved to the background.
  virtual void applicationDidEnterBackground() override;

  // Called when the application reenters the foreground.
  virtual void applicationWillEnterForeground() override;
};

#endif // APP_DELEGATE_H_
