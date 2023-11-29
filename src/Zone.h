#pragma once

#include <string>
#include <nlohmann/json.hpp>

class Zone {
public:

	Zone();
	Zone(const nlohmann::json& j);

	int getId() const;
	std::string getName() const;
	int getX() const;
	int getY() const;
	int getRadius() const;

	void setId(int newId);
	void setName(const std::string& newName);
	void setX(int newX);
	void setY(int newY);
	void setRadius(int newRadius);

private:

	int id;
	std::string name;
	int x, y, radius;

};