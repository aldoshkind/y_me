#pragma once

#include <SFML/System/Vector2.hpp>

#include "geo_point.h"
#include "projector.h"

class geo_object
{
	geo_point				pos;

public:
	/*constructor*/			geo_object				()
	{
		//
	}

	/*destructor*/			~geo_object				()
	{
		//
	}

	geo_point				&get_geopoint			()
	{
		return pos;
	}

	void					set_geopoint			(const geo_point &gp)
	{
		this->pos = gp;
	}

	sf::Vector2f			to_view					(const projector &p) const
	{
		return sf::Vector2f(p.lon_to_x(pos.lon()), p.lat_to_y(pos.lat()));
	}

	void					from_view				(const sf::Vector2f &ps, const projector &p)
	{
		pos = geo_point(p.y_to_lat(ps.y), p.x_to_lon(ps.x));
	}
};
