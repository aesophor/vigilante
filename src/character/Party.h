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
  void addMember(std::shared_ptr<Character> character);
  std::shared_ptr<Character> removeMember(Character* character);

  Character* getLeader() const;
  const std::unordered_set<std::shared_ptr<Character>>& getMembers() const;
  
 protected:
  // `_leader` will NOT be in `_members`.
  Character* _leader;
  std::unordered_set<std::shared_ptr<Character>> _members;
};

}  // namespace vigilante

#endif  // VIGILANTE_PARTY_H_
