#pragma once

#include "projector.h"

class projector_spheric : public projector
{
public:
	/*constructor*/			projector_spheric		()
	{
		//
	}

	virtual /*destructor*/	~projector_spheric		()
	{
		//
	}

	double					x_to_lon				(double x) const
	{
		return 2.0 * M_PI * x - M_PI;
	}

	double					y_to_lat				(double y) const
	{
		y = y * 2.0 * M_PI;
		return 2.0 * atan(exp(M_PI - y)) - M_PI / 2.0;
	}

	double					lon_to_x				(double lon) const
	{
		return (1.0 / 2.0) * (1.0 / M_PI) * (lon + M_PI);
	}

	double					lat_to_y				(double lat) const
	{
		return (1 / 2.0) * (1 / M_PI) * (M_PI - log(tan(M_PI / 4.0 + lat / 2.0)));
	}
};
