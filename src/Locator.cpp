#include "Locator.h"

std::optional<Subscriber> Locator::GetSubscriber(std::string id) {
    auto it = subscribers.find(id);
    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        logger->info("GetSubscriber: Subscriber with ID '{}' found.", id);
        return it->second;
    }
    else {
        logger->warn("GetSubscriber: Subscriber with ID '{}' not found.", id);
        return std::nullopt;
    }
}

void Locator::SetSubscriber(const std::string id, int x, int y) {
    auto it = subscribers.find(id);
    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        int newX = x;
        int newY = y;

        for (const auto& pair : zoneTriggers) {
            const ZoneTrigger& trigger = pair.second;

            if (trigger.getSubID() == id) {
                int foundID = trigger.getID();
                auto subZones = GetZoneSub(trigger.getSubID());
                auto subZonesIt = std::find(subZones.begin(), subZones.end(), trigger.getZoneID());

                switch (trigger.getEvent()) {
                case ZoneTrigger::event::ENTER:
                    if (subZonesIt == subZones.end() && isPointInsideTrigger(newX, newY, trigger)) {
                        logger->info("TriggerZone: ENTER trigger for Subscriber {} in Zone {}.", id, trigger.getZoneID());
                        logger->debug("TriggerZone: Finished the function.");
                    }
                    break;

                case ZoneTrigger::event::EXIT:
                    if (subZonesIt != subZones.end() && !isPointInsideTrigger(newX, newY, trigger)) {
                        logger->info("TriggerZone: EXIT trigger for Subscriber {} in Zone {}.", id, trigger.getZoneID());
                        logger->debug("TriggerZone: Finished the function.");
                    }
                    break;

                case ZoneTrigger::event::ALL:
                    if ((subZonesIt == subZones.end() && isPointInsideTrigger(newX, newY, trigger)) ||
                        (subZonesIt != subZones.end() && !isPointInsideTrigger(newX, newY, trigger))) {
                        logger->info("TriggerZone: ENTER/EXIT trigger for Subscriber {} in Zone {}.", id, trigger.getZoneID());
                        logger->debug("TriggerZone: Finished the function.");
                    }
                    break;
                }
            }
        }

        it->second.setX(x);
        it->second.setY(y);
        logger->info("SetSubscriber: Subscriber with ID '{}' updated at ({}, {}).", id, x, y);

        for (const auto& pair : proxTriggers) {
            const ProximityTrigger& trigger = pair.second;

            if (trigger.getSubscriber1ID() == id || trigger.getSubscriber2ID() == id) {
                auto sub1It = subscribers.find(trigger.getSubscriber1ID());
                auto sub2It = subscribers.find(trigger.getSubscriber2ID());

                if (sub1It != subscribers.end() && sub2It != subscribers.end()) {
                    const Subscriber& sub1 = sub1It->second;
                    const Subscriber& sub2 = sub2It->second;

                    double distanceBetweenPoints = std::hypot(sub2.getX() - sub1.getX(), sub2.getY() - sub1.getY());

                    if (distanceBetweenPoints <= trigger.getDistance()) {
                        logger->info("ProximityTrigger: Triggered for Subscribers {} and {} at distance {} or closer.", trigger.getSubscriber1ID(), trigger.getSubscriber2ID(), trigger.getDistance());
                        logger->debug("ProximityTrigger: Finished the function.");
                    }
                }
                else {
                    logger->warn("ProximityTrigger: One or both Subscribers not found for Trigger with IDs {} and {}.", trigger.getSubscriber1ID(), trigger.getSubscriber2ID());
                }
            }
        }
    }
    else {
        Subscriber newSubscriber;
        newSubscriber.setId(id);
        newSubscriber.setX(x);
        newSubscriber.setY(y);
        subscribers[id] = newSubscriber;
        logger->info("SetSubscriber: New subscriber added with ID '{}' at ({}, {}).", id, x, y);
    }
}

void Locator::load(const std::string& inp) {
    auto logger = logger1.getLogger();

    try {
        nlohmann::json json;
        std::ifstream file(inp);
        file >> json;

        if (json.contains("zones") && json["zones"].is_array()) {
            for (const auto& zoneData : json["zones"]) {
                if (zoneData.contains("id") && zoneData["id"].is_number_integer() &&
                    zoneData.contains("name") && zoneData["name"].is_string() &&
                    zoneData.contains("x") && zoneData["x"].is_number_integer() &&
                    zoneData.contains("y") && zoneData["y"].is_number_integer() &&
                    zoneData.contains("radius") && zoneData["radius"].is_number_integer()) {

                    Zone newZone;
                    newZone.setId(zoneData["id"]);
                    newZone.setName(zoneData["name"]);
                    newZone.setX(zoneData["x"]);
                    newZone.setY(zoneData["y"]);
                    newZone.setRadius(zoneData["radius"]);

                    auto it = zones.find(newZone.getId());

                    if (it == zones.end()) {
                        zones[newZone.getId()] = newZone;
                        logger->info("load: Zone loaded with ID '{}'.", newZone.getId());
                    }
                }
                else {
                    logger->warn("load: Invalid zone data in JSON.");
                }
            }
        }
        else {
            logger->warn("load: 'zones' array not found or invalid in JSON.");
        }
    }
    catch (const std::exception& e) {
        logger->error("load: Error reading JSON: {}", e.what());
    }
}

void Locator::AddZone(int id, std::string name, int x, int y, int radius) {
    auto it = zones.find(id);
    auto logger = logger1.getLogger();

    if (it == zones.end()) {
        Zone newZone;
        newZone.setId(id);
        newZone.setName(name);
        newZone.setX(x);
        newZone.setY(y);
        newZone.setRadius(radius);
        zones[id] = newZone;
        logger->debug("AddZone: Zone added with ID '{}'.", id);
    }
}

void Locator::AddProxTrigger(std::string sub1, std::string sub2, int distance) {
    ProximityTrigger proxTrigger(sub1, sub2, distance);
    proxTriggers[proxTrigger.getID()] = proxTrigger;

    auto logger = logger1.getLogger();
    logger->info("AddProxTrigger: ProximityTrigger added with ID '{}'.", proxTrigger.getID());
}

void Locator::AddZoneTrigger(std::string subID, int zoneID, ZoneTrigger::event event0) {
    ZoneTrigger zoneTrigger(subID, zoneID, event0);
    zoneTriggers[zoneTrigger.getID()] = zoneTrigger;

    auto logger = logger1.getLogger();
    logger->info("AddZoneTrigger: ZoneTrigger added with ID '{}'.", zoneTrigger.getID());
}

std::vector<std::string> Locator::GetSubsInZone(int zoneId) {
    std::vector<std::string> result;
    auto it = zones.find(zoneId);
    auto logger = logger1.getLogger();

    if (it != zones.end()) {
        int centerX = it->second.getX();
        int centerY = it->second.getY();
        int radius = it->second.getRadius();
        for (const auto& subscriber : subscribers) {
            int pointX = subscriber.second.getX();
            int pointY = subscriber.second.getY();
            if (isPointInside(pointX, pointY, centerX, centerY, radius)) {
                result.push_back(subscriber.first);
                logger->debug("GetSubsInZone: Subscriber with ID '{}' is in Zone '{}'.", subscriber.first, zoneId);
            }
        }
    }

    return result;
}

std::vector<int> Locator::GetZoneSub(std::string subId) {
    std::vector<int> result;
    auto it = subscribers.find(subId);
    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        int pointX = it->second.getX();
        int pointY = it->second.getY();
        for (const auto& zone : zones) {
            int centerX = zone.second.getX();
            int centerY = zone.second.getY();
            int radius = zone.second.getRadius();
            if (isPointInside(pointX, pointY, centerX, centerY, radius)) {
                result.push_back(zone.first);
                logger->debug("GetZoneSub: Zone with ID '{}' contains Subscriber '{}'.", zone.first, subId);
            }
        }
    }

    return result;
}

bool Locator::isPointInside(int pointX, int pointY, int centerX, int centerY, int radius) {
    double distance = std::hypot(pointX - centerX, pointY - centerY);
    return distance <= radius;
}

bool Locator::isPointInsideTrigger(int x, int y, const ZoneTrigger& trigger) {
    auto zoneIt = zones.find(trigger.getZoneID());
    if (zoneIt != zones.end()) {
        const Zone& zone = zoneIt->second;
        return isPointInside(x, y, zone.getX(), zone.getY(), zone.getRadius());
    }
    return false;
}
