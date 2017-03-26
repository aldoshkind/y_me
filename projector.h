#pragma once

class projector
{
public:
	/*constructor*/			projector				()
	{
		//
	}

	virtual /*destructor*/	~projector				()
	{
		//
	}

	virtual double			x_to_lon				(double width) = 0;
	virtual double			y_to_lat				(double height) = 0;
	virtual double			lon_to_x				(double lon) = 0;
	virtual double			lat_to_y				(double lat) = 0;
};
