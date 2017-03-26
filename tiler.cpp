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

QPointF						tile::get_coordinates		() const
{
	return coord;
}

QPoint						tile::get_col_row			() const
{
	return colrow;
}

tile::rect_t				tile::get_rect				() const
{
	return rect;
}

void						tile::set_rect				(rect_t rect)
{
	this->rect = rect;
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
