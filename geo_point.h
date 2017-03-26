#pragma once

class geo_point
{
	double lt;
	double ln;

public:
	/*constructor*/				geo_point				(double latitude = 0.0, double longitude = 0.0) : lt(latitude), ln(longitude)
	{
		//
	}

	/*destructor*/				~geo_point				()
	{
		//
	}

	double						lat						() const
	{
		return lt;
	}

	double						lon						() const
	{
		return ln;
	}
};
