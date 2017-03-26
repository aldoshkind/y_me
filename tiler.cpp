#include "tiler.h"

/*contructor*/ tile::tile()
{
	//
}
/*destructor*/ tile::~tile()
{
	//
}

void						tile::set_coordinates		(QPointF c)
{
	coord = c;
}

void						tile::set_col_row			(QPoint col_row)
{
	colrow = col_row;
}

void						tile::set_url				(QString u)
{
	url = u;
}

QPointF						tile::get_coordinates		() const
{
	return coord;
}

QPoint						tile::get_col_row			() const
{
	return colrow;
}

QString						tile::get_url				() const
{
	return "ass";
}







/*contructor*/ tiler::tiler(QString n, int ts)
{
	name = n;
	tile_size = ts;
}

/*destructor*/ tiler::~tiler()
{
	//
}

QString tiler::get_name() const
{
	return name;
}

int tiler::get_tile_size() const
{
	return tile_size;
}
