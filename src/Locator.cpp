#include "Locator.h"

std::optional<Subscriber> Locator::GetSubscriber(std::string id)
{
    auto it = std::find_if(subscribers.begin(), subscribers.end(),
        [&id](const Subscriber& subscriber) {
            return subscriber.getId() == id;
        });

    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        logger->info("GetSubscriber: Subscriber with ID '{}' found.", id);
        return *it;
    }
    else {
        logger->warn("GetSubscriber: Subscriber with ID '{}' not found.", id);
        return std::nullopt;
    }
}

void Locator::SetSubscriber(const std::string id, int x, int y) {
    auto it = std::find_if(subscribers.begin(), subscribers.end(),
        [&id](const Subscriber& subscriber) {
            return subscriber.getId() == id;
        });

    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        it->setX(x);
        it->setY(y);
        logger->info("SetSubscriber: Subscriber with ID '{}' updated at ({}, {}).", id, x, y);
    }
    else {
        Subscriber newSubscriber;
        newSubscriber.setId(id);
        newSubscriber.setX(x);
        newSubscriber.setY(y);
        subscribers.push_back(newSubscriber);
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

                    auto it = std::find_if(zones.begin(), zones.end(),
                        [&newZone](const Zone& zone) {
                            return zone.getId() == newZone.getId();
                        });

                    if (it == zones.end()) {
                        zones.push_back(newZone);
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
    auto it = std::find_if(zones.begin(), zones.end(),
        [&id](const Zone& zone) {
            return zone.getId() == id;
        });

    auto logger = logger1.getLogger();

    if (it == zones.end()) {
        Zone newZone;
        newZone.setId(id);
        newZone.setName(name);
        newZone.setX(x);
        newZone.setY(y);
        newZone.setRadius(radius);
        zones.push_back(newZone);
        logger->info("AddZone: Zone added with ID '{}'.", id);
    }
}

std::vector<std::string> Locator::GetSubsInZone(int zoneId) {
    std::vector<std::string> result;
    auto it = std::find_if(zones.begin(), zones.end(),
        [&zoneId](const Zone& zone) {
            return zone.getId() == zoneId;
        });

    auto logger = logger1.getLogger();

    if (it != zones.end()) {
        int centerX = it->getX();
        int centerY = it->getY();
        int radius = it->getRadius();
        for (const auto& subscriber : subscribers) {
            int pointX = subscriber.getX();
            int pointY = subscriber.getY();
            if (isPointInside(pointX, pointY, centerX, centerY, radius)) {
                result.push_back(subscriber.getId());
                logger->info("GetSubsInZone: Subscriber with ID '{}' is in Zone '{}'.", subscriber.getId(), zoneId);
            }
        }
    }

    return result;
}

std::vector<int> Locator::GetZoneSub(std::string subId) {
    std::vector<int> result;
    auto it = std::find_if(subscribers.begin(), subscribers.end(),
        [&subId](const Subscriber& subscriber) {
            return subscriber.getId() == subId;
        });

    auto logger = logger1.getLogger();

    if (it != subscribers.end()) {
        int pointX = it->getX();
        int pointY = it->getY();
        for (const auto& zone : zones) {
            int centerX = zone.getX();
            int centerY = zone.getY();
            int radius = zone.getRadius();
            if (isPointInside(pointX, pointY, centerX, centerY, radius)) {
                result.push_back(zone.getId());
                logger->info("GetZoneSub: Zone with ID '{}' contains Subscriber '{}'.", zone.getId(), subId);
            }
        }
    }

    return result;
}

bool Locator::isPointInside(int pointX, int pointY, int centerX, int centerY, int radius) {
    double distance = std::hypot(pointX - centerX, pointY - centerY);
    return distance <= radius;
}