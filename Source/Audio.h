// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_AUDIO_H_
#define REQUIEM_AUDIO_H_

#include <filesystem>

namespace requiem {

class Audio {
 public:
  static Audio& the();

  void playSfx(const std::filesystem::path& filePath);
  void playBgm(const std::filesystem::path& filePath);
  void stopBgm();
  void setBgmVolume(const float volume);

 private:
  Audio() = default;
};

}  // namespace requiem

#endif  // REQUIEM_AUDIO_H_
