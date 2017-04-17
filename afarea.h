#pragma once

#include <sstream>

#include <QObject>

#include "additional_math.h"

#include "tree/node.h"
#include "geo_point.h"

class afarea : public QObject, public node
{
	Q_OBJECT

	double val_height;

	void						set_height			(double h)
	{
		val_height = h;
	}

	double						get_height			() const
	{
		return val_height;
	}

public:
	property_get_set<afarea, double>	height;

public:
	/*constructor*/				afarea				() : height("height", this, &afarea::get_height, &afarea::set_height)
	{
		//
	}

	/*destructor*/				~afarea				()
	{
		//
	}

	void						add_vertex			(geo_point pos)
	{
		static int idx = 0;
		std::stringstream ss;
		ss << idx;
		idx += 1;

		if(idx > 3)
		{
			return;
		}

		printf("created\n");

		node *n = append("vx" + ss.str());
		n->add_property(new property_value<double>("latitude"));
		n->add_property(new property_value<double>("longitude"));
		*dynamic_cast<::property<double> *>(n->get_property("latitude")) = DEG(pos.lat());
		*dynamic_cast<::property<double> *>(n->get_property("longitude")) = DEG(pos.lon());
	}

public slots:
	void					slot_mouse_move			(geo_point/* gp*/)
	{
		//
	}

	void					slot_mouse_press		(geo_point gp)
	{
		add_vertex(gp);
	}

	void					slot_mouse_release		(geo_point)
	{
		//
	}
};
