//
// Created by ganim on 07/03/2024.
//

#include "NetWork.h"
#include <fstream>
#include <sstream>
#include <climits>
#include <set>
#include <algorithm>

#define BUS "bus"
#define TRAM "tram"
#define SPRINTER "sprinter"
#define RAIL "rail"
#define INTERCITY "intercity"
#define STAD "stad"
#define CENTRAL "central"

//initialized maps using initializer list
NetWork::NetWork() :
    vehicles({ {BUS,std::make_shared<Vehicle>(2, BUS)},
               {SPRINTER,std::make_shared<Vehicle>(4, SPRINTER)},
               {TRAM,std::make_shared<Vehicle>(3, TRAM)},
               {RAIL,std::make_shared<Vehicle>(5, RAIL)} }),
    stations({ {INTERCITY,std::make_shared<Station>(15, INTERCITY)},
               {STAD,std::make_shared<Station>(10, STAD)},
               {CENTRAL,std::make_shared<Station>(5, CENTRAL)} })
{
}

NetWork::NetWork(const NetWork& other)
{
    operator=(other);
}

NetWork::NetWork(NetWork&& other) noexcept :vehicles(std::move(other.vehicles)),
stations(std::move(other.stations)),lines(std::move(other.lines)),
    nodes(std::move(other.nodes))
{
}

NetWork& NetWork::operator=(const NetWork& other)
{
    if (this != &other)
    {
        this->vehicles = other.vehicles;
        this->lines = other.lines;
        this->stations = other.stations;
        this->nodes = other.nodes;

    }

    return *this;
}

NetWork& NetWork::operator=(NetWork&& other) noexcept {
    this->vehicles = std::move(other.vehicles);
    this->lines = std::move(other.lines);
    this->stations = std::move(other.stations);
    this->nodes = std::move(other.nodes);

    return *this;
}

void NetWork::loadConfiguration(const std::string& s)
{
    std::ifstream file(s);

    if (!file)
    {
        throw invalidConfigFile("file error: " + s + " could not be opened");
    }
    else
    {
        std::string line;
        while (std::getline(file, line)) //inserting each line containing type and num into lines
        {
            std::string type;
            int num;
            std::istringstream stream(line); //inputing the type and time

            if (!(stream >> type >> num))
            {
                throw invalidConfigFile("config file error: invalid format");
            }
            if (vehicles.find(type) != vehicles.end()) //checks if vehicle type is in map
            {
                vehicles[type]->set_stopTime(num);//we set the transit of the type
            }
            else if (stations.find(type) != stations.end()) //otherwise check in stations map if it exists
            {
                stations[type]->setTransit(num);//we set the transit of the type
            }
            else
            {
                throw invalidConfigFile("type doesnt exist ");//otherwise type doesnt exists
            }
        }
    }

}

void NetWork::loadLines(const std::string& f)
{
    std::weak_ptr<Vehicle> vehicle;

    for (const auto& pair : vehicles)
    {
        if (f.rfind(pair.first, 0) == 0)
        {
            vehicle = pair.second; //initialization of weak ptr from the shared ptr of the vehicle object
        }
    }

    if (vehicle.expired())
    {
        throw invalidLineFile("invalid file name"); //checking if the file is in the desired format
    }

    std::ifstream file(f);

    if (!file)
    {
        throw invalidLineFile("file error: cant be opened");
    }
    else
    {
        std::string line;

        while (std::getline(file, line))
        {
            std::string src;
            std::string targ;
            int duration;
            std::istringstream stream(line);//inputting the src and target and the duration

            if (!(stream >> src >> targ >> duration))
            {
                throw invalidLineFile("file output error: invalid format");
            }

            if (duration < 0)
            {
                throw invalidLineFile("file output error: negative duration");
            }

            addNodeIfNeeded(src); //added to the node map
            addNodeIfNeeded(targ);//added to the node map

            int curr_dur = lines[nodes[src]][nodes[targ]][vehicle];

            // to update the duration if the duration doesn't exist or if the duration of the line
            if (curr_dur == 0 || duration < curr_dur)
            {
                //is less than the current duration and update it if so
                lines[nodes[src]][nodes[targ]][vehicle] = duration;
            }
        }
    }
}

//responsible for adding the nodes to the node map
bool NetWork::addNodeIfNeeded(const std::string& node)
{
    std::weak_ptr<Station> station;
    if (node.rfind("IC", 0) == 0) {//meaning it is a prefix of the node string and the prefix was for an INTERCITY
        station = stations[INTERCITY];
    }
    else if (node.rfind("CS", 0) == 0) {//meaning it is a prefix of the node string and the prefix was for a central
        station = stations[CENTRAL];
    }
    else {
        station = stations[STAD];// the prefix was for a stad
    }
    if (nodes.find(node) == nodes.end()) {
        nodes[node] = std::make_shared<Node>(station, node);
        return true;
    }
    return false;
}

NetWork::out_bound_map NetWork::outbound(const std::string& point) const
{
    return out_bound_with_lines(point, lines);
}

NetWork::linesMap NetWork::transpose_line() const
{
    linesMap linesmap;

    for (const auto& node_pair : lines)
    {
        for (const auto& node_vechile_pair : node_pair.second)
        {
            for (const auto& vehicle_distance_pair : node_vechile_pair.second)
            {
                linesmap[node_vechile_pair.first][node_pair.first][vehicle_distance_pair.first]
                    = vehicle_distance_pair.second;
            }
        }
    }

    return linesmap;
}

//here we mapped the nodes that we can get from a certain node with a specfic vehicle
NetWork::out_bound_map NetWork::out_bound_with_lines(const std::string& point,
    const NetWork::linesMap& dest_line) const
{
    if (nodes.find(point) == nodes.end())
    {
        throw std::invalid_argument(point + " does not exist in the current network");
    }

    //here i preformed a dfs for indicating the nodes reachable from a certain node
    // this map is responsible for maping the vehicle type to the nodes they can reach
    out_bound_map vehicle_to_nodes;

    for (const auto& vehicle_pair : vehicles)
    {
        vehicle_to_nodes[vehicle_pair.second] = std::vector<std::weak_ptr<Node>>();
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> visited; //the visted nodes
        std::vector<std::weak_ptr<Node>> stack;

        // add starting position to stack
        stack.push_back(nodes.at(point));

        while (!stack.empty())
        {
            std::weak_ptr<Node> node = stack.back();
            stack.pop_back();

            if (visited.count(node) == 0) // not visited
            {
                visited.insert(node);

                if (node.lock()->getName() != point) // add if different than starting position
                {
                    vehicle_to_nodes[vehicle_pair.second].push_back(node);
                }

                // iterate over neighbors of popped node (if it has any neighbors)
                if (dest_line.count(node) > 0)
                {
                    for (const auto& node_pair : dest_line.at(node))
                    {
                        // neighbor linked using specified vehicle
                        if (node_pair.second.count(vehicle_pair.second) != 0)
                        {
                            stack.push_back(node_pair.first);
                        }
                    }
                }
            }
        }
    }

    return vehicle_to_nodes;
}
//here we sent the reversed lines as a param for out_bound_with_lines to get the nodes of the source that are connected to our point;
NetWork::inbound_map NetWork::inbound(const std::string& point) const
{

    return out_bound_with_lines(point, (transpose_line()));
}

NetWork::express_map NetWork::uniExpress(const std::string& src, const std::string& targ) const
{
    if (nodes.find(src) == nodes.end()) //if src not in nodes then throw exception
    {
        throw std::invalid_argument(src + " does not exist in the current network");
    }
    if (nodes.find(targ) == nodes.end()) //if dest not in nodes then throw exception
    {
        throw std::invalid_argument(targ + " does not exist in the current network");
    }

    express_map express; //final dijkstara run result map

    for (const auto& vehicle_pair : vehicles) {
        std::map<std::weak_ptr<Node>, int, std::owner_less<std::weak_ptr<Node>>> distance; //map indcating distances between
        std::map<std::weak_ptr<Node>, std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> prev; //not visted
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> node_q; //our "queue" for visted

        for (const auto& node_pair : nodes)   //intallizing all the nodes to INT_MAX
        {
            distance[node_pair.second] = INT_MAX;
            prev[node_pair.second] = std::weak_ptr<Node>();
            node_q.insert(node_pair.second);
        }

        distance[nodes.at(src)] = 0; //begining of dijkastra run

        while (node_q.size() > 0) {
            std::weak_ptr<Node> minNode;

            for (const std::weak_ptr<Node>& node : node_q) {
                if (minNode.expired() || distance.at(node) < distance.at(minNode)) {
                    minNode = node;
                }
            }

            node_q.erase(minNode);

            // min node has outbound lines, and it is reachable from source
            if (lines.count(minNode) > 0 && distance.at(minNode) < INT_MAX)
            {
                for (const auto& node_pair : lines.at(minNode))
                {
                    if (node_pair.second.count(vehicle_pair.second) > 0) {
                        int newDistance = distance.at(minNode) + node_pair.second.at(vehicle_pair.second); // neighbor distance= get the value of the min(current distance,or the minNode to the neighbor)

                        // used lock for getting access to shared_ptr that its associated with;
                        if (minNode.lock()->getName() != targ
                            && minNode.lock()->getName() != src)
                        {
                            newDistance += vehicle_pair.second->get_stopTime();
                        }
                        if (newDistance < distance.at(node_pair.first))
                        {
                            distance[node_pair.first] = newDistance;
                            prev[node_pair.first] = minNode;
                        }
                    }
                }
            }
        }

        express[vehicle_pair.second] = distance.at(nodes.at(targ));
    }

    return express;
}

// finds time of short path from src to target passing through mid, starting at curNode,
// with preVehicle point to the last used vehicle in path (an empty pointer if curNode is the first node in path).
// if mid is an empty string, this constraint is not imposed
int NetWork::multiShortestPathFindHelper(const std::weak_ptr<Node>& curNode, int timeSoFar,
    const std::string& target, const std::string& src, const std::string& mid,
    std::set<std::tuple<std::string, std::string, std::string>>& visitedEdges,
    const std::weak_ptr<Vehicle>& prevVehicle) const
{
    // reached target, having passed throuh mid
    if (curNode.lock()->getName() == target && mid.empty())
    {
        return timeSoFar;
    }

    // other halting condition is when all edges in graph were marked

    std::string newMid;

    if (curNode.lock()->getName() == mid) // mid found
    {
        newMid = std::string(); // mark as empty for next call
    }
    else
    {
        newMid = mid; // keep current mid
    }

    int minTime = INT_MAX; // look for min time

    // cur node has outbound lines
    if (lines.count(curNode) > 0)
    {
        for (const auto& nodePair : lines.at(curNode))
        {
            for (const auto& vehiclePair : nodePair.second)
            {
                std::tuple<std::string, std::string, std::string> edgeTuple(curNode.lock()->getName(),
                    nodePair.first.lock()->getName(),
                    vehiclePair.first.lock()->getName());

                if (visitedEdges.count(edgeTuple) == 0) // edge not visited yet
                {
                    // mark edge as visited
                    visitedEdges.insert(edgeTuple);

                    int newTime = timeSoFar + vehiclePair.second; // add line time to distance so far
                    //if using the same vehicle at the stop time, else add transit time

                    // used lock for getting access to shared_ptr that its associated with
                    if (curNode.lock()->getName() != src)
                    {
                        if (vehiclePair.first.lock()->getName()
                            == prevVehicle.lock()->getName())
                        {
                            newTime += vehiclePair.first.lock()->get_stopTime();
                        }
                        else
                        {
                            newTime += curNode.lock()->getStation().lock()->getTransit();
                        }
                    }

                    int resTime = multiShortestPathFindHelper(nodePair.first, newTime, target, src, newMid,
                        visitedEdges, vehiclePair.first);

                    minTime = std::min(minTime, resTime);

                    // mark edge as unvisited
                    visitedEdges.erase(edgeTuple);
                }
            }
        }
    }

    return minTime;
}

int NetWork::multiExpress(const std::string& src, const std::string& targ) const
{
    if (nodes.find(src) == nodes.end()) //if src not in nodes then throw exception
    {
        throw std::invalid_argument(src + " does not exist in the current network");
    }
    if (nodes.find(targ) == nodes.end()) //if dest not in nodes then throw exception
    {
        throw std::invalid_argument(targ + " does not exist in the current network");
    }

    // initialize visited set as empty, time to 0, and prev vehicle to an empty pointer
    std::set<std::tuple<std::string, std::string, std::string>> visitedEdges;

    // no mid constraint
    return multiShortestPathFindHelper(nodes.at(src), 0, targ, src, std::string(),
        visitedEdges, std::weak_ptr<Vehicle>());
}

int NetWork::viaExpress(const std::string& src, const std::string& transitNode,
    const std::string& targ) const
{
    if (nodes.find(transitNode) == nodes.end())
    {
        throw std::invalid_argument(transitNode + " does not exist in the current network");
    }

    // handle edge case where src, transit and targ are equal
    if (src == transitNode && src == targ)
    {
        return 0;
    }

    // initialize visited set as empty, time to 0, and prev vehicle to an empty pointer
    std::set<std::tuple<std::string, std::string, std::string>> visitedEdges;

    // with mid constraint
    return multiShortestPathFindHelper(nodes.at(src), 0, targ, src, transitNode,
        visitedEdges, std::weak_ptr<Vehicle>());
}

std::ostream& operator<<(std::ostream& out, const NetWork& other)
{
    out << "Vehicles:" << std::endl;

    for (const auto& vehicle_pair : other.vehicles)
    {
        out << *(vehicle_pair.second) << std::endl;
    }

    out << std::endl << "Nodes:" << std::endl;
    for (const auto& node_pair : other.nodes)
    {
        out << *(node_pair.second) << std::endl;
    }

    out << std::endl << "Lines:" << std::endl;

    // iterate over lines
    for (const auto& node_pair : other.lines)
    {
        out << std::endl << "outgoing from: " << *(node_pair.first.lock()) << std::endl;

        for (const auto& node_pair2 : node_pair.second)
        {
            out << "to " << *(node_pair2.first.lock()) << ": ";

            for (const auto& vehicle_pair : node_pair2.second)
            {
                out << "(" << *(vehicle_pair.first.lock()) << "," << vehicle_pair.second << ") ";
            }

            out << std::endl;
        }
    }

    return out;
}

NetWork::invalidConfigFile::invalidConfigFile(const std::string& exc) : invalid_argument(exc)
{
}

NetWork::invalidLineFile::invalidLineFile(const std::string& exc) : invalid_argument(exc)
{
}
