// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "MainMenuScene.h"

#include <axmol.h>

#include "Assets.h"
#include "Audio.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/Colorscheme.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

bool MainMenuScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto winSize = Director::getInstance()->getWinSize();

  // Initialize background ImageView.
  _background = ui::ImageView::create(string{vigilante::assets::kMainMenuBg});
  _background->setAnchorPoint({0, 0});
  addChild(_background);

  // Initialize labels.
  for (int i = 0; i < static_cast<int>(Option::SIZE); i++) {
    Label* label = Label::createWithTTF(string(_kOptionStr[i]), string{assets::kBoldFont}, assets::kRegularFontSize);
    label->getFontAtlas()->setAliasTexParameters();
    label->setPosition(winSize.width / 2, winSize.height / 2 - _kMenuOptionGap * (i + 1));
    addChild(label);
    _labels.push_back(label);
  }
  _current = 0;
  _labels[_current]->setTextColor(vigilante::colorscheme::kRed);

  // Initialize footer labels.
  Label* copyrightLabel = Label::createWithTTF(_kCopyrightStr, string{assets::kBoldFont}, assets::kRegularFontSize);
  copyrightLabel->setAnchorPoint({0.5, 0});
  copyrightLabel->setPosition(winSize.width / 2, copyrightLabel->getContentSize().height + _kFooterLabelPadding);
  addChild(copyrightLabel);

  Label* versionLabel = Label::createWithTTF(_kVersionStr, string{assets::kBoldFont}, assets::kRegularFontSize);
  versionLabel->setAnchorPoint({1, 0});
  versionLabel->setPosition(winSize.width - _kFooterLabelPadding, versionLabel->getContentSize().height + _kFooterLabelPadding);
  addChild(versionLabel);

  // Initialize InputManager.
  InputManager::the().activate(this);

  // Play main menu Bgm.
  Audio::the().playBgm(assets::kMainThemeBgm);

  schedule(AX_SCHEDULE_SELECTOR(MainMenuScene::update));
  return true;
}

void MainMenuScene::update(float) {
  handleInput();
}

void MainMenuScene::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    if (_current == 0) {
      return;
    }
    _labels[_current--]->setTextColor(vigilante::colorscheme::kWhite);
    _labels[_current]->setTextColor(vigilante::colorscheme::kRed);
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    if (_current == static_cast<int>(Option::SIZE) - 1) {
      return;
    }
    _labels[_current++]->setTextColor(vigilante::colorscheme::kWhite);
    _labels[_current]->setTextColor(vigilante::colorscheme::kRed);
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    switch (static_cast<Option>(_current)) {
      case Option::NEW_GAME: {
        Audio::the().stopBgm();
        InputManager::the().deactivate();
        SceneManager::the().pushScene(GameScene::create());
        SceneManager::the().getCurrentScene<GameScene>()->startNewGame();
        break;
      }
      case Option::LOAD_GAME: {
        Audio::the().stopBgm();
        InputManager::the().deactivate();
        SceneManager::the().pushScene(GameScene::create());
        SceneManager::the().getCurrentScene<GameScene>()->loadGame("quicksave.vgs");
        break;
      }
      case Option::OPTIONS:
        break;
      case Option::EXIT:
        Director::getInstance()->end();
        break;
      default:
        break;
    }
  }
}

}  // namespace vigilante
