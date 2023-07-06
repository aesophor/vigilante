// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Audio.h"

//#include <SimpleAudioEngine.h>

using namespace std;
//using namespace CocosDenshion;

namespace vigilante {

Audio& Audio::the() {
  static Audio instance;
  return instance;
}

void Audio::playBgm(const string& filename) {
  constexpr bool loop = true;
  //SimpleAudioEngine::getInstance()->playBackgroundMusic(filename.c_str(), loop);
}

void Audio::stopBgm() {
  //SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void Audio::playSfx(const string& filename) {
  constexpr bool loop = false;
  //SimpleAudioEngine::getInstance()->playEffect(filename.c_str(), loop);
}

void Audio::setBgmVolume(const float volume) {
  //SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

void Audio::setSfxVolume(const float volume) {
  //SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

}  // namespace vigilante
