#include <gtest/gtest.h>
#include "Locator.h"
#include "Triggers.h"

TEST(GetSubscriber, ReturnNulloptIfSubDoesNotExist) {
    Locator locator;
    auto result = locator.GetSubscriber("unknown");
    ASSERT_FALSE(result.has_value());
}


TEST(GetSubscriber, ReturnSubsciberDataIfSuscriberInDb)
{
    Locator locator;
    locator.SetSubscriber("+79115555555", 10, 20);
    auto result = locator.GetSubscriber("+79115555555");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->getX(), 10);
    ASSERT_EQ(result->getY(), 20);
}

TEST(GetSubscriber, ReturnLastSubsciberData)
{
    Locator locator;
    locator.SetSubscriber("+79115555555", 10, 20);
    locator.SetSubscriber("+79115555555", -55, -77);
    auto result = locator.GetSubscriber("+79115555555");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->getX(), -55);
    ASSERT_EQ(result->getY(), -77);
}

TEST(LoadZones, ReturnLoadedZones)
{
    Locator locator;
    locator.load("config.json");
    auto result = locator.zones;
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0].getName(), "Zone1");
    ASSERT_EQ(result[1].getName(), "Zone2");
}

TEST(GetSubsInZone, ReturnSubscriberInZone)
{
    Locator locator;
    locator.AddZone(1000, "spb", 0, 0, 20);
    locator.SetSubscriber("+79115555555", 1, 2);
    auto result = locator.GetSubsInZone(1000);
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), "+79115555555");
}

TEST(GetZoneSub, ReturnZoneSub)
{
    Locator locator;
    locator.AddZone(1000, "spb", 0, 0, 20);
    locator.SetSubscriber("+79115555555", 1, 2);
    auto result = locator.GetZoneSub("+79115555555");
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), 1000);
}

TEST(TriggerDistance, ReturnTriggerDistance)
{
    Locator locator;
    locator.SetSubscriber("+79115555555", 1, 2);
    locator.SetSubscriber("+79114444444", 11, 2);
    Triggers trigger;
    auto result1 = trigger.TriggerDistance(locator, 10);
    locator.SetSubscriber("+79114444444", 101, 2);
    auto result2 = trigger.TriggerDistance(locator, 10);
    ASSERT_EQ(result1.size(), 2);
    ASSERT_EQ(result2.size(), 0);
}

TEST(TriggerZone, ReturnTriggerZone)
{
    Locator locator;
    locator.SetSubscriber("+79115555555", 1, 2);
    locator.AddZone(1, "spb", 0, 0, 10);
    Triggers trigger;
    auto result1 = trigger.TriggerZone(locator, "+79115555555", 1, 20, 20);
    locator.SetSubscriber("+79114444444", 20, 25);
    auto result2 = trigger.TriggerZone(locator, "+79114444444", 1, 1, 2);
    ASSERT_EQ(result1, EXIT);
    ASSERT_EQ(result2, ENTER);
}