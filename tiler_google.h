#pragma once

#include "tiler.h"
#include "projector_spheric.h"

#define GOOGLE_TILE_WIDTH			256.0
#define GOOGLE_TILE_HEIGHT			256.0

class tiler_google : public tiler
{
	projector_spheric			p;
	double						width_to_lon				(double width, int zoom);
	double						height_to_lat				(double height, int zoom);
	double						lon_to_width				(double lon, int zoom);
	double						lat_to_height				(double lat, int zoom);
	int							get_col						(double lon, int zoom);
	int							get_row						(double lat, int zoom);
	QPoint						get_tile_col_row			(geo_point world_coord, int zoom);

public:
	/*contructor*/				tiler_google				();
	/*destructor*/				~tiler_google				();

	QString						get_name			();
	tiles_t						get_tiles_for		(QPointF tl, QPointF br, int zoom);
	tiles_t						get_tiles_for		(geo_point tl, geo_point br, int zoom);
	tiles_t						get_tiles_for		(double lon_left, double lon_right, double lat_top, double lat_bottom, int zoom);
	double						get_tile_size		(int zoom) const;
	double						get_base_tile_size	() const;
	QString						get_url				(int col, int row, int zoom) const;
};
