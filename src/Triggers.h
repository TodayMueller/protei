#pragma once

#include <string>
#include <vector>

#include "Locator.h"
#include "spdlog/spdlog.h"

enum event { ENTER, EXIT, ENTER_EXIT };

class Triggers {
public:
    std::vector<std::string> TriggerDistance(Locator& locator, double distance);
    enum event TriggerZone(Locator& locator, std::string subId, int zoneId, int newX, int newY);
};