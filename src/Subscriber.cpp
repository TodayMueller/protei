#include "Subscriber.h"

int Subscriber::getX() const {
    return x;
}
int Subscriber::getY() const {
    return y;
}
std::string Subscriber::getId() const {
    return id;
}


void Subscriber::setId(const std::string& newId) {
    id = newId;
}
void Subscriber::setX(int newX) {
    x = newX;
}
void Subscriber::setY(int newY) {
    y = newY;
}
