#include "MainMenuScene.h"

#include "GameAssetManager.h"
#include "MainGameScene.h"
#include "ui/Colorscheme.h"
#include "util/Letterbox.h"

using std::array;
using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::Scene;
using cocos2d::Label;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::GameInputManager;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFontSize;

const array<string, MainMenuScene::Option::SIZE> MainMenuScene::_kOptionStr = {{
  "New Game",
  "Load Game",
  "Options",
  "Exit"
}};

MainMenuScene::~MainMenuScene() {}

bool MainMenuScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto winSize = Director::getInstance()->getWinSize();

  // Initialize background ImageView.
  _background = ImageView::create(vigilante::asset_manager::kMainMenuBg);
  _background->setAnchorPoint({0, 0});
  addChild(_background);

  // Initialize labels.
  for (int i = 0; i < static_cast<int>(Option::SIZE); i++) {
    Label* label = Label::createWithTTF(_kOptionStr[i], kBoldFont, kRegularFontSize);
    label->getFontAtlas()->setAliasTexParameters();
    label->setPosition(winSize.width / 2, winSize.height / 2 - 25 * (i + 1));
    addChild(label);
    _labels.push_back(label);
  }
  _current = 0;
  _labels[_current]->setTextColor(vigilante::colorscheme::kRed);

  // Initialize GameInputManager.
  _inputMgr = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _inputMgr->activate(this);

  schedule(schedule_selector(MainMenuScene::update));
  return true;
}

void MainMenuScene::update(float delta) {
  handleInput(delta);
}

void MainMenuScene::handleInput(float delta) {
  if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    if (_current == 0) {
      return;
    }
    _labels[_current--]->setTextColor(vigilante::colorscheme::kWhite);
    _labels[_current]->setTextColor(vigilante::colorscheme::kRed);
  } else if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    if (_current == static_cast<int>(Option::SIZE) - 1) {
      return;
    }
    _labels[_current++]->setTextColor(vigilante::colorscheme::kWhite);
    _labels[_current]->setTextColor(vigilante::colorscheme::kRed);
  } else if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    switch (static_cast<Option>(_current)) {
      case Option::NEW_GAME: {
        Scene* scene = MainGameScene::create();
        Director::getInstance()->pushScene(scene);
        break;
      }
      case Option::LOAD_GAME:
        break;
      case Option::OPTIONS:
        break;
      case Option::EXIT:
        break;
      default:
        break;
    }
  }
}
