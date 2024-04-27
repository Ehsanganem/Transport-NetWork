//
// Created by ganim on 07/03/2024.
//

#include "Vehicle.h"

Vehicle::Vehicle(int stop_time, const std::string& name):stop_time(stop_time), name(name)
{
}

int Vehicle::get_stopTime() const 
{
    return this->stop_time;
}

void Vehicle::set_stopTime(int time) 
{
    this->stop_time=time;
}

const std::string &Vehicle::getName() const
{
    return this->name;
}

std::ostream &operator<<(std::ostream &out, const Vehicle &other)
{
    return out << other.name << "(" << other.stop_time << ")";
}
