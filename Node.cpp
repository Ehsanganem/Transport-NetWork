//
// Created by ganim on 07/03/2024.
//

#include "Node.h"

Node::Node(const std::weak_ptr<Station>& station,const std::string& name)
    :station(station), name(name)
{
}

const std::string &Node::getName() const 
{
    return this->name;
}

std::weak_ptr<Station> Node::getStation() const
{
    return this->station;
}

std::ostream &operator<<(std::ostream &out, const Node &other) 
{
    return out << other.name << "(" << (*other.station.lock()) << ")";
}
