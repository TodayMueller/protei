#include "Triggers.h"

std::vector<std::string> Triggers::TriggerDistance(Locator& locator, double distance) {
    std::vector<std::string> result;

    auto logger = locator.logger1.getLogger();

    logger->debug("TriggerDistance: Starting the function.");

    for (auto it1 = locator.subscribers.begin(); it1 != locator.subscribers.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != locator.subscribers.end(); ++it2) {
            int point1X = it1->getX();
            int point1Y = it1->getY();

            int point2X = it2->getX();
            int point2Y = it2->getY();

            double distanceBetweenPoints = std::hypot(point2X - point1X, point2Y - point1Y);

            if (std::abs(distanceBetweenPoints - distance) < 1e-6) {
                result.push_back(it1->getId());
                result.push_back(it2->getId());

                logger->warn("TriggerDistance: Triggered for Subscribers {} and {} at distance {}.", it1->getId(), it2->getId(), distance);
            }
        }
    }

    logger->debug("TriggerDistance: Finished the function.");

    return result;
}


event Triggers::TriggerZone(Locator& locator, std::string subId, int zoneId, int newX, int newY) {
    auto logger = locator.logger1.getLogger();

    logger->debug("TriggerZone: Starting the function.");

    auto itSub = std::find_if(locator.subscribers.begin(), locator.subscribers.end(),
        [&subId](const Subscriber& subscriber) {
            return subscriber.getId() == subId;
        });

    auto itZone = std::find_if(locator.zones.begin(), locator.zones.end(),
        [&zoneId](const Zone& zone) {
            return zone.getId() == zoneId;
        });

    if (itSub != locator.subscribers.end() && itZone != locator.zones.end()) {
        auto subZones = locator.GetZoneSub(subId);
        auto subZonesIt = std::find(subZones.begin(), subZones.end(), zoneId);

        if (subZonesIt != subZones.end()) {
            if (!locator.isPointInside(newX, newY, itZone->getX(), itZone->getY(), itZone->getRadius())) {
                logger->warn("TriggerZone: EXIT trigger for Subscriber {} in Zone {}.", subId, zoneId);

                logger->debug("TriggerZone: Finished the function.");

                return EXIT;
            }
        }
        else {
            if (locator.isPointInside(newX, newY, itZone->getX(), itZone->getY(), itZone->getRadius())) {
                logger->warn("TriggerZone: ENTER trigger for Subscriber {} in Zone {}.", subId, zoneId);

                logger->debug("TriggerZone: Finished the function.");

                return ENTER;
            }
        }
    }

    logger->info("TriggerZone: EXIT_ENTER trigger for Subscriber {} in Zone {}.", subId, zoneId);
    logger->debug("TriggerZone: Finished the function.");

    return ENTER_EXIT;
}
