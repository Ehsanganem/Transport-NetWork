//
// Created by ganim on 07/03/2024.
//

#ifndef HOMEWORK2_VEHICLE_H
#define HOMEWORK2_VEHICLE_H

#include <string>
#include <ostream>

class Vehicle {
private:
    int stop_time;
    std::string name;

public:
    Vehicle(int stop_time,const std::string& name);

    //getter and setter
    int get_stopTime() const;
    void set_stopTime(int time);
    const std::string& getName()const;

    friend std::ostream& operator<<(std::ostream& out,const Vehicle& other);
};

#endif //HOMEWORK2_VEHICLE_H
