#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <optional>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Zone.h"
#include "Subscriber.h"
#include "Logger.h"

class Locator {
public:

    std::vector<Subscriber> subscribers;
    std::vector<Zone> zones;
    Logger logger1 = Logger("config.json");
    
    void SetSubscriber(const std::string id, int x, int y);
    std::optional<Subscriber> GetSubscriber(const std::string);

    void load(const std::string& filename);
    void AddZone(int id, std::string name, int x, int y, int radius);

    std::vector<std::string> GetSubsInZone(int zoneId);
    std::vector<int> GetZoneSub(std::string);

    bool isPointInside(int pointX, int pointY, int centerX, int centerY, int radius);

};
