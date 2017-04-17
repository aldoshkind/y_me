#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <jsoncpp/json/json.h>

#include "map_view.h"
#include "tree/widget.h"
#include "tree/node.h"
#include "yuneec/route.h"

#include "afarea.h"

class main_widget : public QWidget
{
	Q_OBJECT

	QHBoxLayout			*layout_main;
	QVBoxLayout			*layout_structure;
	QHBoxLayout			*layout_buttons;
	QVBoxLayout			*layout_map;
	QHBoxLayout			*layout_info;
	QLabel				*label_coords;
	QLabel				*label_zoom;

	map_view			*sfml;
	Widget				*tree_widget;

	QPushButton			*button_open;
	QPushButton			*button_save;

	node				root;
	yuneec::route		route;

	afarea				area;

	void				load				(QString fn)
	{
		route.load(fn.toStdString());
	}

	int save(QString path)
	{
		return route.save(path.toStdString()) == true ? 0 : -1;
	}

public:
	/*constructor*/		main_widget			() : QWidget(NULL)
	{
		layout_main = new QHBoxLayout(this);

		layout_map = new QVBoxLayout;

		layout_info = new QHBoxLayout;
		sfml = new map_view(this);
		sfml->get_route().set_tree(&root);
		sfml->get_afarea_render().set_tree(&root);

		label_coords = new QLabel(this);
		label_zoom = new QLabel(this);

		tree_widget = new Widget(this);
		tree_widget->set_tree(&root);
		button_open = new QPushButton("Open", this);
		button_save = new QPushButton("Save", this);
		layout_structure = new QVBoxLayout;
		layout_buttons = new QHBoxLayout;
		layout_buttons->addWidget(button_open);
		layout_buttons->addWidget(button_save);

		layout_info->addWidget(label_coords);
		layout_info->addWidget(label_zoom);

		layout_map->addWidget(sfml, 1);
		layout_map->addLayout(layout_info, 0);

		layout_structure->addWidget(tree_widget);
		layout_structure->addLayout(layout_buttons);

		layout_main->addLayout(layout_structure, 1);
		layout_main->addLayout(layout_map, 2);

		connect(sfml, SIGNAL(signal_mouse_move(geo_point)), this, SLOT(slot_mouse_coords(geo_point)));
		connect(sfml, SIGNAL(signal_zoom_level(int)), this, SLOT(slot_zoom(int)));

		connect(button_open, SIGNAL(pressed()), this, SLOT(slot_load()));
		connect(button_save, SIGNAL(pressed()), this, SLOT(slot_save()));

		root.attach("route", &route, false);

		route.attach("afarea", &area, false);

		node *wps = route.append("waypoints");

		wps->insert("test", new yuneec::waypoint);
		wps->insert("test0", new yuneec::waypoint);
		wps->insert("test1", new yuneec::waypoint, "test");

		connect(sfml, SIGNAL(signal_mouse_press(geo_point)), &area, SLOT(slot_mouse_press(geo_point)));
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

	void				slot_load			()
	{
		QString fn = QFileDialog::getOpenFileName(this, tr("Open rps"), "/home/dmitry", tr("*.rps"));
		if(fn.size() != 0)
		{
			load(fn);
		}
	}
	void				slot_save			()
	{
		QString fn = QFileDialog::getSaveFileName(this, tr("Save rps"), "/home/dmitry", tr("*.rps"));
		if(fn.size() != 0)
		{
			save(fn);
		}
	}
};
