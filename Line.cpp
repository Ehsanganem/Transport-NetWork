//
// Created by ganim on 07/03/2024.
//

#include "Line.h"

Line::Line(const std::shared_ptr<Node> &src, 
	const std::shared_ptr<Node>& dest,int duration) : src(src),dest(dest),duration(duration)
{
}
