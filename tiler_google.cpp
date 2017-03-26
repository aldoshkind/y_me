#include "tiler_google.h"

#include <QDebug>

#include "math_common.h"

template <class value_type>
inline value_type clamp(const value_type &v, const value_type &lower, const value_type &upper)
{
	return v > upper ? upper : (v < lower ? lower : v);
}

/*constructor*/ tiler_google::tiler_google() : tiler("google", 1024)
{

}

/*destructor*/ tiler_google::~tiler_google()
{

}

double tiler_google::width_to_lon(double width, int zoom)
{
	width = width / (GOOGLE_TILE_WIDTH * pow(2, zoom));
	return DEG(p.x_to_lon(width));
}

double tiler_google::height_to_lat(double height, int zoom)
{
	height = height / (GOOGLE_TILE_HEIGHT * pow(2, zoom));
	return DEG(p.y_to_lat(height));
}

double tiler_google::lon_to_width(double lon, int zoom)
{
	double width = p.lon_to_x(RAD(lon));
	return GOOGLE_TILE_WIDTH * pow(2.0, zoom) * width;
}

double tiler_google::lat_to_height(double lat, int zoom)
{
	double height = p.lat_to_y(RAD(lat));
	return GOOGLE_TILE_HEIGHT * pow(2.0, zoom) * height;
}

int tiler_google::get_col(double lon, int zoom)
{
	return lon_to_width(lon, zoom) / GOOGLE_TILE_WIDTH;
}

int tiler_google::get_row(double lat, int zoom)
{
	return lat_to_height(lat, zoom) / GOOGLE_TILE_HEIGHT;
}

QPoint tiler_google::get_tile_col_row(geo_point world_coord, int zoom)
{
	//qDebug() << world_coord.lat() << world_coord.lon();

	int col = lon_to_width(world_coord.lon(), zoom) / GOOGLE_TILE_WIDTH;
	int row = lat_to_height(world_coord.lat(), zoom) / GOOGLE_TILE_HEIGHT;

	return QPoint(col, row);
}

QString tiler_google::get_url(int col, int row, int zoom) const
{
	// m - maps - vector
	// t - terrain - relief
	// s - satellite
	// r - roadmap
	// h - hybrid
	// p - roadmap jpeg

	return QString("http://mt.google.com/vt/lyrs=s&hl=ru&x=%1&y=%2&z=%3").arg(col).arg(row).arg(zoom);
}

tiler::tiles_t tiler_google::get_tiles_for(QPointF tl, QPointF br, int zoom)
{
	tiles_t res;

	double tile_size = get_tile_size(zoom) / get_base_tile_size();
	int tile_count = get_base_tile_size() / get_tile_size(zoom);

	int bottom = br.y() / tile_size + 1;
	int top = tl.y() / tile_size;

	int left = tl.x() / tile_size - (tl.x() < 0.0 ? 1 : 0);
	int right = br.x() / tile_size + 1;

	top = clamp(top, 0, tile_count);
	//left = clamp(left, 0, tile_count);
	//right = clamp(right, 0, tile_count);
	bottom = clamp(bottom, 0, tile_count);

	//qDebug() <<  right - left << "x" << bottom - top;

	for(int i = left ; i < right ; i += 1)
	{
		for(int j = top ; j < bottom ; j += 1)
		{
			tile t;
			double width = i * tile_size;
			double height = j * tile_size;

			t.set_coordinates(QPointF(width, height));

			int col = ((abs(i) / tile_count + 1) * tile_count + i) % tile_count;
			int row = ((abs(j) / tile_count + 1) * tile_count + j) % tile_count;

			t.set_col_row(QPoint(col, row));

			double w = tile_size * tile_count;

			tile::rect_t rect;
			rect.push_back(geo_point(p.y_to_lat(height / w), p.x_to_lon(width / w)));
			rect.push_back(geo_point(p.y_to_lat(height / w), p.x_to_lon((width + tile_size) / w)));
			rect.push_back(geo_point(p.y_to_lat((height + tile_size) / w), p.x_to_lon((width + tile_size) / w)));
			rect.push_back(geo_point(p.y_to_lat((height + tile_size) / w), p.x_to_lon(width / w)));

			t.set_rect(rect);

			res.push_back(t);
		}
	}
	//qDebug() << res.size();

	return res;
}

QString tiler_google::get_name()
{
	return "google";
}

double tiler_google::get_tile_size(int zoom) const
{
	return get_base_tile_size() / pow(2.0, zoom);
}

double tiler_google::get_base_tile_size() const
{
	return 256.0;
}

tiler::tiles_t tiler_google::get_tiles_for(geo_point gtl, geo_point gbr, int zoom)
{
	QPointF tl(p.lon_to_x(gtl.lon()), p.lat_to_y(clamp(gtl.lat(), -1.4, 1.4)));
	QPointF br(p.lon_to_x(gbr.lon()), p.lat_to_y(clamp(gbr.lat(), -1.4, 1.4)));
	return get_tiles_for(tl, br, zoom);
}

tiler::tiles_t tiler_google::get_tiles_for(double lon_left, double lon_right, double lat_top, double lat_bottom, int zoom)
{
	QPointF tl(p.lon_to_x(lon_left), p.lat_to_y(clamp(lat_top, -1.48, 1.48)));
	QPointF br(p.lon_to_x(lon_right), p.lat_to_y(clamp(lat_bottom, -1.48, 1.48)));
	return get_tiles_for(tl, br, zoom);
}
