// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PARTY_H_
#define VIGILANTE_PARTY_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace vigilante {

// Forward declaration
class Character;

class Party {
 public:
  struct WaitingLocationInfo {
    std::string tmxMapFileName;
    float x;
    float y;
  };

  explicit Party(Character* leader);
  virtual ~Party() = default;

  Character* getMember(const std::string& characterJsonFileName) const;
  bool hasMember(const std::string& characterJsonFileName) const;
  void recruit(Character* targetCharacter);
  void dismiss(Character* targetCharacter, bool addToMap=true);

  void askMemberToWait(Character* targetCharacter);  // wait in a specific map
  void askMemberToFollow(Character* targetCharacter);  // resume following


  bool hasWaitingMember(const std::string& characterJsonFileName) const;
  void addWaitingMember(const std::string& characterJsonFileName,
                        const std::string& currentTmxMapFileName,
                        float x,
                        float y);
  void removeWaitingMember(const std::string& characterJsonFileName);
  Party::WaitingLocationInfo getWaitingMemberLocationInfo(const std::string& characterJsonFileName) const;

  Character* getLeader() const;
  std::unordered_set<Character*> getLeaderAndMembers() const;
  const std::unordered_set<std::shared_ptr<Character>>& getMembers() const;
  const std::unordered_map<std::string, Party::WaitingLocationInfo>& getWaitingMembersLocationInfo() const;

 protected:
  void addMember(std::shared_ptr<Character> character);
  std::shared_ptr<Character> removeMember(Character* character);

  // `_leader` will NOT be in `_members`.
  Character* _leader;
  std::unordered_set<std::shared_ptr<Character>> _members;
  std::unordered_map<std::string, Party::WaitingLocationInfo> _waitingMembersLocationInfo;
};

}  // namespace vigilante

#endif  // VIGILANTE_PARTY_H_
