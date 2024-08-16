// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "RoomRentalTracker.h"

#include "util/Logger.h"

using namespace std;

namespace vigilante {

bool RoomRentalTracker::hasCheckedIn(const std::string& innTmxFilePath) const {
  return _checkedInInns.contains(innTmxFilePath);
}

bool RoomRentalTracker::checkIn(const string& innTmxFilePath) {
  const auto it = _checkedInInns.find(innTmxFilePath);
  if (it != _checkedInInns.end()) {
    VGLOG(LOG_ERR, "Failed to checkin [%s], err: [already checked in].", innTmxFilePath.c_str());
    return false;
  }

  _checkedInInns.emplace(innTmxFilePath);
  return true;
}

bool RoomRentalTracker::checkOut(const string& innTmxFilePath) {
  const auto it = _checkedInInns.find(innTmxFilePath);
  if (it == _checkedInInns.end()) {
    VGLOG(LOG_ERR, "Failed to checkout [%s], err: [already checked out].", innTmxFilePath.c_str());
    return false;
  }

  return _checkedInInns.erase(innTmxFilePath);
}

}  // namespace vigilante
