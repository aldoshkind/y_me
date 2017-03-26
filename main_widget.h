#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "map_view.h"

class main_widget : public QWidget
{
	Q_OBJECT

	QVBoxLayout			*layout_main;
	QHBoxLayout			*layout_info;
	QLabel				*label_coords;
	QLabel				*label_zoom;

	map_view			*sfml;

public:
	/*constructor*/		main_widget			() : QWidget(NULL)
	{
		layout_main = new QVBoxLayout(this);

		layout_info = new QHBoxLayout;
		sfml = new map_view(this);

		label_coords = new QLabel(this);
		label_zoom = new QLabel(this);

		layout_info->addWidget(label_coords);
		layout_info->addWidget(label_zoom);

		layout_main->addWidget(sfml, 1);
		layout_main->addLayout(layout_info, 0);

		connect(sfml, SIGNAL(signal_mouse_move(geo_point)), this, SLOT(slot_mouse_coords(geo_point)));
		connect(sfml, SIGNAL(signal_zoom_level(int)), this, SLOT(slot_zoom(int)));
	}

	/*destructor*/		~main_widget			()
	{
		//
	}

private slots:
	void				slot_mouse_coords	(geo_point gp)
	{
		label_coords->setText(QString("%1 %2").arg(DEG(gp.lat())).arg(DEG(gp.lon())));
	}

	void				slot_zoom			(int zoom)
	{
		label_zoom->setText(QString("Z: %1").arg(zoom));
	}
};
