// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_GAMEPLAY_ROOM_RENTAL_TRACKER_H_
#define REQUIEM_GAMEPLAY_ROOM_RENTAL_TRACKER_H_

#include <string>
#include <unordered_set>

namespace requiem {

class RoomRentalTracker final {
 public:
  bool hasCheckedIn(const std::string& innTmxFilePath) const;
  bool checkIn(const std::string& innTmxFilePath);
  bool checkOut(const std::string& innTmxFilePath);

  inline const std::unordered_set<std::string>& getCheckedInInns() const { return _checkedInInns; }
  inline void setCheckedInInns(std::unordered_set<std::string>&& checkedInInns) {
    _checkedInInns = std::move(checkedInInns);
  }

 private:
  std::unordered_set<std::string> _checkedInInns;
};

}  // namespace requiem

#endif  // REQUIEM_GAMEPLAY_ROOM_RENTAL_TRACKER_H_
