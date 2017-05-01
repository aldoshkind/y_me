#pragma once

#include <sstream>

#include <QObject>

#include <SFML/Graphics.hpp>

#include "additional_math.h"

#include "tree/node.h"
#include "geo_point.h"
#include "projector_spheric.h"
#include "yuneec/yuneec_route.h"
#include "yuneec/yuneec_waypoint.h"

class afarea : public QObject, public node, public property_listener
{
	Q_OBJECT

	double				val_height;
	projector_spheric	proj;

	void						set_height			(double h);
	double						get_height			() const;

	typedef sf::Rect<double>			rect;
	typedef std::vector<sf::Vector2f>	contour;

	rect						get_bbox			(const contour &c) const;
	contour						get_contour			() const;
	void						update_route		();

	node						*route;

	void						updated				(property_base *prop);

public:
	property_get_set<afarea, double>	height;

public:
	/*constructor*/				afarea				();
	/*destructor*/				~afarea				();

	void						set_route			(node *route);
	void						add_vertex			(geo_point pos);

public slots:
	void					slot_mouse_move			(geo_point/* gp*/);
	void					slot_mouse_press		(geo_point gp);
	void					slot_mouse_release		(geo_point);
};
