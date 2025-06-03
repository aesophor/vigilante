// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_CHARACTER_PARTY_H_
#define REQUIEM_CHARACTER_PARTY_H_

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace requiem {

// Forward declaration
class Character;

class Party final {
  friend class GameState;

 public:
  struct WaitingLocationInfo {
    std::string tmxMapFilePath;
    float x;
    float y;
  };

  explicit Party(Character* leader) : _leader{leader} {}

  Character* getMember(const std::string& characterJsonFilePath) const;
  bool hasMember(const std::string& characterJsonFilePath) const;
  void recruit(Character* targetCharacter);
  void dismiss(Character* targetCharacter, bool addToMap=true);
  void dismissAll(bool addToMap=true);

  void askMemberToWait(Character* targetCharacter);
  void askMemberToWait(Character* targetCharacter,
                       const std::string& tmxMapFilePath,
                       const float x,
                       const float y);
  void askMemberToFollow(Character* targetCharacter);

  void addWaitingMember(const std::string& characterJsonFilePath,
                        const std::string& tmxMapFilePath,
                        const float x,
                        const float y);
  void removeWaitingMember(const std::string& characterJsonFilePath);

  std::optional<Party::WaitingLocationInfo>
  getWaitingMemberLocationInfo(const std::string& characterJsonFilePath) const;

  inline const std::unordered_set<std::shared_ptr<Character>>& getMembers() const { return _members; }
  inline Character* getLeader() const { return _leader; }
  std::unordered_set<Character*> getLeaderAndMembers() const;

  inline const std::unordered_map<std::string, Party::WaitingLocationInfo>&
  getWaitingMembersLocationInfos() const {
    return _waitingMembersLocationInfos;
  }

  void dump();

 protected:
  void addMember(std::shared_ptr<Character> character);
  std::shared_ptr<Character> removeMember(Character* character);

  // `_leader` will NOT be in `_members`.
  Character* _leader{};
  std::unordered_set<std::shared_ptr<Character>> _members;
  std::unordered_map<std::string, Party::WaitingLocationInfo> _waitingMembersLocationInfos;
};

}  // namespace requiem

#endif  // REQUIEM_CHARACTER_PARTY_H_
