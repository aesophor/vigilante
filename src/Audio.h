// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_AUDIO_H_
#define VIGILANTE_AUDIO_H_

#include <string>

namespace vigilante {

class Audio {
 public:
  static Audio* getInstance();

  void playBgm(const std::string& filename);
  void stopBgm();

  void playSfx(const std::string& filename);

  void setBgmVolume(const float volume);
  void setSfxVolume(const float volume);

 private:
  Audio() = default;
};

}  // namespace vigilante

#endif  // VIGILANTE_AUDIO_H_
