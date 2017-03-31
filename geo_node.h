#pragma once

#include "geo_object.h"
#include "render_node.h"
#include "projector.h"

class geo_node : public geo_object
{
	sf::Transformable			*n;

public:
	/*constructor*/				geo_node				(sf::Transformable *n)
	{
		this->n = n;
	}

	/*destructor*/				geo_node				()
	{
		//
	}

	void						map						(const projector &proj)
	{
		n->setPosition(to_view(proj));
	}

	sf::Transformable			*get					() const
	{
		return n;
	}
};
