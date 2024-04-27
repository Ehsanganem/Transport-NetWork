//
// Created by ganim on 07/03/2024.
//

#ifndef HOMEWORK2_LINE_H
#define HOMEWORK2_LINE_H
#include "Node.h"
#include <memory>

class Line {
private:
    std::weak_ptr<Node> src;//for memory handling
    std::weak_ptr<Node> dest;//for memory handling
    int duration;// duration to get from src to dest
public:
    Line(const std::shared_ptr<Node>& src,const std::shared_ptr<Node>& dest,int duration);

};


#endif //HOMEWORK2_LINE_H
