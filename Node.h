//
// Created by ganim on 07/03/2024.
//

#ifndef HOMEWORK2_NODE_H
#define HOMEWORK2_NODE_H
#include <memory>
#include "Station.h"
#include <ostream>
class Node {
private:
    std::weak_ptr<Station> station;
    std::string name;

public:
    Node(const std::weak_ptr<Station>& station, const std::string& name);

    const std::string& getName( )const;
    std::weak_ptr<Station> getStation() const;

    friend std::ostream& operator<<(std::ostream& out, const Node& other);
};


#endif //HOMEWORK2_NODE_H
