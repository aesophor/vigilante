// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PARTY_H_
#define VIGILANTE_PARTY_H_

#include <memory>
#include <string>
#include <unordered_set>

namespace vigilante {

// Forward declaration
class Character;

class Party {
 public:
  explicit Party(Character* leader);
  virtual ~Party() = default;

  bool hasMember(const std::string& characterJsonFileName) const;
  void recruit(Character* targetCharacter);
  void dismiss(Character* targetCharacter, bool addToMap=true);

  bool hasDeceasedMember(const std::string& characterJsonFileName) const;
  void addDeceasedMember(const std::string& characterJsonFileName);
  void removeDeceasedMember(const std::string& characterJsonFileName);

  Character* getLeader() const;
  std::unordered_set<Character*> getLeaderAndMembers() const;
  const std::unordered_set<std::shared_ptr<Character>>& getMembers() const;
  const std::unordered_set<std::string>& getDeceasedMembers() const;

 protected:
  void addMember(std::shared_ptr<Character> character);
  std::shared_ptr<Character> removeMember(Character* character);

  // `_leader` will NOT be in `_members`.
  Character* _leader;
  std::unordered_set<std::shared_ptr<Character>> _members;
  std::unordered_set<std::string> _deceasedMembers;
};

}  // namespace vigilante

#endif  // VIGILANTE_PARTY_H_
