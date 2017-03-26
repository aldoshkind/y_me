#pragma once

#include <QPointF>
#include <QString>
#include <QVector>
#include <QGeoCoordinate>

class tile
{
	QPointF						coord;
	QPoint						colrow;
	QString						url;

public:
	/*contructor*/				tile				();
	/*destructor*/				~tile				();

	void						set_coordinates		(QPointF coord);
	void						set_col_row			(QPoint col_row);
	void						set_url				(QString url);

	QPointF						get_coordinates		() const;
	QPoint						get_col_row			() const;
	QString						get_url				() const;

};

class tiler
{
public:
	/*contructor*/				tiler				(QString name, int tile_size);
	virtual /*destructor*/		~tiler				();

	QString						get_name			() const;
	int							get_tile_size		() const;
	virtual QVector<tile>		get_tiles_for		(QPointF tl, QPointF br, int zoom) = 0;
	virtual double				get_tile_size		(int zoom) const = 0;
	virtual double				get_base_tile_size	() const = 0;
	virtual QString				get_url				(int col, int row, int zoom) const = 0;

private:
	QString						name;
	int							tile_size;
};
