// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_AUDIO_MANAGER_H_
#define VIGILANTE_AUDIO_MANAGER_H_

#include <string>

namespace vigilante {

class AudioManager {
 public:
  static AudioManager* getInstance();

  void playBgm(const std::string& filename);
  void stopBgm();

  void playSfx(const std::string& filename);

 private:
  AudioManager() = default;
};

}  // namespace vigilante

#endif  // VIGILANTE_AUDIO_MANAGER_H_
