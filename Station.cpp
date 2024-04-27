//
// Created by ganim on 07/03/2024.
//

#include "Station.h"

Station::Station(int transit, const std::string &name):name(name),transit(transit)
{
}

int Station::getTransit() 
{
    return this->transit;
}

void Station::setTransit(int transit)
{
    this->transit=transit;
}

std::ostream &operator<<(std::ostream &out, const Station &other) 
{
    return out << other.name << "(" << other.transit << ")";
}
