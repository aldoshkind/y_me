#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include <SFML/Graphics.hpp>

#include "geo_node.h"

class route
{
	std::vector<geo_node>	nodes;

public:
	/*constructor*/			route			()
	{
		//
	}

	/*destructor*/			~route			()
	{
		//
	}

	void					draw			(sf::RenderTarget, projector *p)
	{
		for(auto node : nodes)
		{
			node.map(p);
		}
	}
};

#endif // ROUTE_H
