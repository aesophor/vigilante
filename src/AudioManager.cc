// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "AudioManager.h"

#include <SimpleAudioEngine.h>

using namespace std;
using namespace CocosDenshion;

namespace vigilante {

AudioManager* AudioManager::getInstance() {
  static AudioManager instance;
  return &instance;
}

void AudioManager::playBgm(const string& filename) {
  constexpr bool loop = true;
  SimpleAudioEngine::getInstance()->playBackgroundMusic(filename.c_str(), loop);
}

void AudioManager::stopBgm() {
  SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void AudioManager::playSfx(const string& filename) {
  constexpr bool loop = false;
  SimpleAudioEngine::getInstance()->playEffect(filename.c_str(), loop);
}

void AudioManager::setBgmVolume(const float volume) {
  SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

void AudioManager::setSfxVolume(const float volume) {
  SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

}  // namespace vigilante
