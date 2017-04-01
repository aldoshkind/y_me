#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <jsoncpp/json/json.h>

#include "map_view.h"
#include "tree/widget.h"
#include "tree/node.h"

class main_widget : public QWidget
{
	Q_OBJECT

	QHBoxLayout			*layout_main;
	QVBoxLayout			*layout_map;
	QHBoxLayout			*layout_info;
	QLabel				*label_coords;
	QLabel				*label_zoom;

	map_view			*sfml;
	Widget				*tree_widget;

	node				root;

	void				load				()
	{
		Json::Value mission;

		std::ifstream mission_file("/home/dmitry/yuneec.rps", std::ifstream::binary);
		setlocale(LC_ALL, "C");
		mission_file >> mission;

		std::cout << mission;

		Json::Value &json_route = mission["route"];
		Json::Value &json_waypoints = mission["waypoints"];
		if(json_route.isNull() == true || json_waypoints.isNull() == true)
		{
			return;
		}
		node *n_route = root.append("route");
		for(auto prop_name : json_route.getMemberNames())
		{
			auto prop = new property_value<std::string>(prop_name);
			n_route->add_property(prop);
			*prop = json_route[prop_name].asString();
		}

		node *n_waypoints = n_route->append("waypoints");
		for(Json::ArrayIndex i = 0 ; i < json_waypoints.size() ; i += 1)
		{
			std::stringstream ss;
			ss << i;
			node *n = n_waypoints->append(ss.str());
			Json::Value &j_waypoint = json_waypoints[i];
			for(auto prop_name : j_waypoint.getMemberNames())
			{
				const Json::Value &wp_prop = j_waypoint[prop_name];
				property_base *prop = NULL;

				switch(wp_prop.type())
				{
				case Json::intValue:
					{
						property_value<int> *p = new property_value<int>(prop_name);
						*p = wp_prop.asInt();
						prop = p;
					}
				break;
				case Json::realValue:
					{
						property_value<double> *p = new property_value<double>(prop_name);
						*p = wp_prop.asDouble();
						prop = p;
					}
				break;
				case Json::stringValue:
					{
						property_value<std::string> *p = new property_value<std::string>(prop_name);
						*p = wp_prop.asString();
						prop = p;
					}
				break;
				default:
					continue;
				break;
				};
				n->add_property(prop);
			}
		}

	}

public:
	/*constructor*/		main_widget			() : QWidget(NULL)
	{
		layout_main = new QHBoxLayout(this);

		layout_map = new QVBoxLayout(this);

		layout_info = new QHBoxLayout;
		sfml = new map_view(this);
		sfml->get_route().set_tree(&root);

		label_coords = new QLabel(this);
		label_zoom = new QLabel(this);

		tree_widget = new Widget(this);
		tree_widget->set_tree(&root);

		layout_info->addWidget(label_coords);
		layout_info->addWidget(label_zoom);

		layout_map->addWidget(sfml, 1);
		layout_map->addLayout(layout_info, 0);

		layout_main->addWidget(tree_widget, 1);
		layout_main->addLayout(layout_map, 3);

		connect(sfml, SIGNAL(signal_mouse_move(geo_point)), this, SLOT(slot_mouse_coords(geo_point)));
		connect(sfml, SIGNAL(signal_zoom_level(int)), this, SLOT(slot_zoom(int)));

		load();
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
