#ifndef VIGILANTE_MAIN_MENU_SCENE_H_
#define VIGILANTE_MAIN_MENU_SCENE_H_

#include <array>
#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UIImageView.h"

#include "input/GameInputManager.h"

class MainMenuScene : public cocos2d::Scene {
 public:
  CREATE_FUNC(MainMenuScene);
  virtual ~MainMenuScene();

  virtual bool init() override;
  virtual void update(float delta) override;
  void handleInput(float delta);

 private:
  enum Option {
    NEW_GAME,
    LOAD_GAME,
    OPTIONS,
    EXIT,
    SIZE
  };
  static const std::array<std::string, MainMenuScene::Option::SIZE> _kOptionStr;

  cocos2d::ui::ImageView* _background;
  std::vector<cocos2d::Label*> _labels;
  int _current;

  std::unique_ptr<vigilante::GameInputManager> _inputMgr;
};

#endif // VIGILANTE_MAIN_MENU_SCENE_H_
