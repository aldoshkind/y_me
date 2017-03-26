#include "math_elliptic.h"

#include <math.h>

#include "math_common.h"

double elliptic_lon_to_width(double lon)
{
	return (1.0 / 2.0) * (1.0 / M_PI) * (lon + M_PI);
}

double elliptic_lat_to_height(double lat)
{	
	// b - малая полуось, а - большая.
	double a = 6378137.000;
	double b = 6356752.3142;
	double eccentr = sqrt(1.0 - (b / a) * (b / a));
	double e_sin_lat = eccentr * sin(lat);
	double coef = ((1.0 - e_sin_lat) / (1.0 + e_sin_lat));
	coef = pow(coef, eccentr / 2.0);
	
	return (1 / 2.0) * (1 / M_PI) * (M_PI - log(tan(M_PI / 4.0 + lat / 2.0) * coef));
}

double elliptic_width_to_lon(double x)
{
	return x / ((1 / 2.0) * (1 / M_PI)) - M_PI;
}
 
double elliptic_height_to_lat(double y)
{
	double ts = exp(y / ((1 / 2.0) * (1 / M_PI)) - M_PI);
	double phi = M_PI_2 - 2.0 * atan(ts);
	double dphi = 1.0;
	
	double a = 6378137.000;
	double b = 6356752.3142;
	double eccentr = sqrt(1.0 - (b / a) * (b / a));
	
	int i = 0;
	for(i = 0 ; fabs(dphi) > 0.000000001 && i < 30 ; i += 1)
	{
		double con = eccentr * sin (phi);
		dphi = M_PI_2 - 2 * atan(ts * pow((1.0 - con) / (1.0 + con), eccentr / 2.0)) - phi;
		phi += dphi;
	}
	return phi;
}
