#pragma once

class projector
{
	double scale;

public:
	/*constructor*/			projector				()
	{
		scale = 1.0;
	}

	virtual /*destructor*/	~projector				()
	{
		//
	}

	virtual double			x_to_lon				(double width) const = 0;
	virtual double			y_to_lat				(double height) const = 0;
	virtual double			lon_to_x				(double lon) const = 0;
	virtual double			lat_to_y				(double lat) const = 0;
	void					set_scale				(double scale)
	{
		this->scale = scale;
	}

	double					get_scale				() const
	{
		return scale;
	}
};
