#include "math_spheric.h"

#include <math.h>

#include "math_common.h"

double spheric_lon_to_width(double lon)
{
	return (1.0 / 2.0) * (1.0 / M_PI) * (lon + M_PI);
}

double spheric_lat_to_height(double lat)
{
	return (1 / 2.0) * (1 / M_PI) * (M_PI - log(tan(M_PI / 4.0 + lat / 2.0)));
}

double spheric_width_to_lon(double width)
{
	return 2.0 * M_PI * width - M_PI;
}

double spheric_height_to_lat(double height)
{
	height = height * 2.0 * M_PI;
	return 2.0 * atan(exp(M_PI - height)) - M_PI / 2.0;
}
