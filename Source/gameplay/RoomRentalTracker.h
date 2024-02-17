// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_GAMEPLAY_ROOM_RENTAL_TRACKER_H_
#define VIGILANTE_GAMEPLAY_ROOM_RENTAL_TRACKER_H_

#include <string>
#include <unordered_set>

namespace vigilante {

class RoomRentalTracker final {
 public:
  bool checkIn(const std::string& innTmxFilePath);
  bool checkOut(const std::string& innTmxFilePath);

 private:
  std::unordered_set<std::string> _checkedInInns;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAMEPLAY_ROOM_RENTAL_TRACKER_H_
