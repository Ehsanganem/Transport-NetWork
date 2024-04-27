//
// Created by ganim on 07/03/2024.
//

#ifndef HOMEWORK2_NETWORK_H
#define HOMEWORK2_NETWORK_H

#include "Node.h"
#include "Vehicle.h"
#include "Station.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
class NetWork {

public:
    typedef std::map<std::weak_ptr<Vehicle>, std::vector<std::weak_ptr<Node>>, std::owner_less<std::weak_ptr<Vehicle>>> out_bound_map;
    typedef out_bound_map inbound_map;
    typedef std::map<std::weak_ptr<Vehicle>,
        int, std::owner_less<std::weak_ptr<Vehicle>>> express_map;


private:
    typedef std::map<std::weak_ptr<Node>, std::map<std::weak_ptr<Node>, std::map<std::weak_ptr<Vehicle>, int, std::owner_less<std::weak_ptr<Vehicle>>>
        , std::owner_less<std::weak_ptr<Node>>>, std::owner_less<std::weak_ptr<Node>>> linesMap;

    // data members
    std::map<std::string, std::shared_ptr<Vehicle>> vehicles; // the map connects between the type of Vehicle
    // to the Vehicle object and the the stop time for each vehicle
    std::map<std::string, std::shared_ptr<Station>> stations; // the stations map connects between the type name of the station
    //to the station object
    linesMap lines; // a map the connects between the src nodes and
    //all nodes that the source has a destination to (used owner_less here for comparison of weak_ptrs based
    // on the shared_ptr comparison )
    std::map<std::string, std::shared_ptr<Node>> nodes;//map to locate nodes based on name

    // methods
    bool addNodeIfNeeded(const std::string& node);
    linesMap transpose_line() const;// we reverse the lines a help function for the second required outbound function
    out_bound_map out_bound_with_lines(const std::string& point, const linesMap& dest_line) const;

    int multiShortestPathFindHelper(const std::weak_ptr<Node>& curNode, int timeSoFar,
        const std::string& target, const std::string& src, const std::string& mid,
        std::set<std::tuple<std::string, std::string, std::string>>& visitedEdges,
        const std::weak_ptr<Vehicle>& prevVehicle) const;

public:
    // error classes
    class invalidConfigFile :public std::invalid_argument {
    public:
        explicit invalidConfigFile(const std::string& exc);
    };

    class invalidLineFile :public std::invalid_argument {
    public:
        explicit invalidLineFile(const std::string& exc);
    };

    NetWork();
    NetWork(const NetWork& other);
    NetWork(NetWork&& other) noexcept ;
    NetWork& operator=(const NetWork& other);
    NetWork& operator=(NetWork&& other) noexcept ;

    out_bound_map outbound(const std::string& point) const;
    inbound_map inbound(const std::string& point) const;
    void loadConfiguration(const std::string&);
    void loadLines(const std::string& f);
    express_map uniExpress(const std::string& src, const std::string& targ) const;
    int multiExpress(const std::string& src, const std::string& targ) const;
    int viaExpress(const std::string& src, const std::string& transitNode, const std::string& targ) const;
    friend std::ostream& operator<<(std::ostream& out, const NetWork& other);
};

#endif //HOMEWORK2_NETWORK_H
