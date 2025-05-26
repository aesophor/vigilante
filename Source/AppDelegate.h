// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_APP_DELEGATE_H_
#define REQUIEM_APP_DELEGATE_H_

#include <axmol.h>

// The axmol Application.
// Private inheritance here hides part of interface from Director.
class AppDelegate final : private ax::Application {
 public:
  AppDelegate() = default;
  virtual ~AppDelegate() = default;

  virtual void initGLContextAttrs() override;

  // Implement Director and Scene init here.
  virtual bool applicationDidFinishLaunching() override;

  // Called when the application is moved to the background.
  virtual void applicationDidEnterBackground() override;

  // Called when the application reenters the foreground.
  virtual void applicationWillEnterForeground() override;
};

#endif  // REQUIEM_APP_DELEGATE_H_
