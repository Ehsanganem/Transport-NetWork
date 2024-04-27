//
// Created by ganim on 07/03/2024.
//

#ifndef HOMEWORK2_STATION_H
#define HOMEWORK2_STATION_H


#include <string>
#include <ostream>

class Station {
private:
    std::string name;
    int transit;

public:
    Station(int transit,const std::string& name);

    //getter and setters;
    int getTransit();
    void setTransit(int transit);

    friend std::ostream& operator<<(std::ostream& out,const Station& other);
};


#endif //HOMEWORK2_STATION_H
