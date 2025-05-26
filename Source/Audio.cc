// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Audio.h"

#include <audio/AudioEngine.h>

namespace fs = std::filesystem;
using namespace std;
USING_NS_AX;

namespace requiem {

namespace {

AUDIO_ID bgmAudioId{};
float bgmVolume{1.0f};

}  // namespace

Audio& Audio::the() {
  static Audio instance;
  return instance;
}

void Audio::playSfx(const fs::path& filePath) {
  AudioEngine::play2d(filePath.c_str(), /*loop=*/false);
}

void Audio::playBgm(const fs::path& filePath) {
  if (bgmAudioId) {
    stopBgm();
    bgmAudioId = {};
  }

  bgmAudioId = AudioEngine::play2d(filePath.c_str(), /*loop=*/true, /*volume=*/bgmVolume);
}

void Audio::stopBgm() {
  AudioEngine::stop(bgmAudioId);
}

void Audio::setBgmVolume(const float volume) {
  bgmVolume = volume;
  AudioEngine::setVolume(bgmAudioId, bgmVolume);
}

}  // namespace requiem
