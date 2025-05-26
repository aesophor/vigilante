// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_STATIC_ACTOR_H_
#define REQUIEM_STATIC_ACTOR_H_

#include <filesystem>
#include <vector>

#include <axmol.h>
#include <2d/Node.h>

namespace requiem {

// A static actor is an abstract class which represents a game entity
// consisting of the following members:
// 1. a sprite placed at a specific position
// 2. a spritesheet and several animations
class StaticActor {
 public:
  virtual ~StaticActor() {
    _node->release();
  }

  // Show and hide the sprite in the game map.
  // showOnMap() and removeFromMap() both return a bool
  // which indicates if the operation is successful.
  virtual bool showOnMap(float x, float y);
  virtual bool removeFromMap();
  virtual void setPosition(float x, float y);

  inline ax::Node* getNode() const { return _node; }
  inline ax::Sprite* getBodySprite() const { return _bodySprite; }
  inline ax::SpriteBatchNode* getBodySpritesheet() const { return _bodySpritesheet; }

  // Create animation from Texture/{category}/{entityName}/{entityName}_{frameName}
  // e.g., to create the animation of "slime" "killed", pass the following arguments
  // to this function: ("Texture/character/slime", "killed", 3.0f / kPpm).
  //
  // If the target animation cannot be created, the fallback animation will be used
  // instead. If the user did not provide a fallback animation, a std::runtime_error
  // will be thrown.
  //
  // IMPORTANT: animations created with this utility method should be release()d!
  //
  // @param textureResDirPath: the path to texture resource directory
  // @param framesName: the name of the frames
  // @param interval: the interval between each frame
  // @param fallback: the fallback animation (if the target animation cannot be created)
  // @return: an pointer to the instance of ax::Animation
  static ax::Animation* createAnimation(const std::filesystem::path& textureResDirPath,
                                        const std::string& framesName,
                                        const float interval,
                                        ax::Animation* fallback = nullptr);

  // The texture resources under Resources/Texture/ has the following rules:
  //
  // Texture/character/player/player_attacking0/0.png
  // |______________________| |____| |________| |___|
  //    textureResDirPath        |   framesName
  //                      framesNamePrefix
  //
  // As you can see, each framesName (e.g., attacking0) is preceded by a prefix,
  // this is to **prevent frames name collision** in ax::SpriteFrameCache!
  //
  // The following utility method takes in textureResDirPath as a parameter
  // and returns the last dirname.
  // e.g., "Texture/character/player" -> "player"
  static std::string getLastDirName(const std::filesystem::path& directoryPath);

  // Example: Texture/fx/dust/spritesheet.png
  //          |_____________| |_____________|
  //         textureResDirPath
  static std::filesystem::path getSpritesheetFilePath(const std::filesystem::path& textureResDirPath);

 protected:
  explicit StaticActor(const size_t numAnimations = 1)
      : _node{ax::Node::create()},
        _bodyAnimations(numAnimations) {
    _node->retain();
  }

  bool _isShownOnMap{};
  ax::Node* _node{};
  ax::Sprite* _bodySprite{};
  ax::SpriteBatchNode* _bodySpritesheet{};
  std::vector<ax::Animation*> _bodyAnimations;
};

}  // namespace requiem

#endif  // REQUIEM_STATIC_ACTOR_H_
