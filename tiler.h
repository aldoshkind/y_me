#pragma once

#include <QPointF>
#include <QString>
#include <QVector>
#include <QGeoCoordinate>

class tile
{
public:
	typedef std::vector<QGeoCoordinate>		rect_t;

private:
	QPointF						coord;
	QPoint						colrow;
	rect_t						rect;

public:
	/*contructor*/				tile				();
	/*destructor*/				~tile				();

	void						set_coordinates		(QPointF coord);
	void						set_col_row			(QPoint col_row);
	void						set_rect			(rect_t rect);

	QPointF						get_coordinates		() const;
	QPoint						get_col_row			() const;
	rect_t						get_rect			() const;
};

class tiler
{
public:
	/*contructor*/				tiler				(QString name, int tile_size);
	virtual /*destructor*/		~tiler				();

	typedef std::vector<tile>	tiles_t;

	QString						get_name			() const;
	int							get_tile_size		() const;
	virtual tiles_t				get_tiles_for		(QPointF tl, QPointF br, int zoom) = 0;
	virtual double				get_tile_size		(int zoom) const = 0;
	virtual double				get_base_tile_size	() const = 0;
	virtual QString				get_url				(int col, int row, int zoom) const = 0;
	virtual tiles_t				get_tiles_for		(QGeoCoordinate tl, QGeoCoordinate br, int zoom) = 0;
	virtual tiles_t				get_tiles_for		(double lon_left, double lon_right, double lat_top, double lat_bottom, int zoom) = 0;

private:
	QString						name;
	int							tile_size;
};
