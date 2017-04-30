#include "afarea.h"

/*constructor*/ afarea::afarea() : height("height", this, &afarea::get_height, &afarea::set_height)
{
	this->route = NULL;
}

/*destructor*/ afarea::~afarea()
{
	//
}

void afarea::set_route(node *route)
{
	this->route = route;
}

/*rect						get_bbox			(const contour &c) const
{
	//
}*/

void afarea::add_vertex(geo_point pos)
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


	if(idx > 2)
	{
		update_route();
	}
}



void afarea::slot_mouse_move(geo_point/* gp*/)
{
	//
}

void afarea::slot_mouse_press(geo_point gp)
{
	add_vertex(gp);
}

void afarea::slot_mouse_release(geo_point)
{
	//
}

afarea::rect afarea::get_bbox(const contour &c) const
{
	double t = std::numeric_limits<double>::max();
	double l = std::numeric_limits<double>::max();
	double r = -l;
	double b = -t;
	for(auto v : c)
	{
		if(t > v.y)
		{
			t = v.y;
		}
		if(l > v.x)
		{
			l = v.x;
		}
		if(b < v.y)
		{
			b = v.y;
		}
		if(r < v.x)
		{
			r = v.x;
		}
	}
	rect res;
	res.left = l;
	res.top = t;
	res.width = r - l;
	res.height = b - t;

	return res;
}

afarea::contour afarea::get_contour() const
{
	contour res;
	for(auto name : ls())
	{
		const ::property<double> *plat = dynamic_cast<const ::property<double> *>(at(name)->get_property("latitude"));
		const ::property<double> *plon = dynamic_cast<const ::property<double> *>(at(name)->get_property("longitude"));

		double lat = *plat;
		double lon = *plon;

		double x = proj.lon_to_x(RAD(lon));
		double y = proj.lat_to_y(RAD(lat));

		res.push_back(sf::Vector2f(x, y));
	}
	return res;
}

void afarea::update_route()
{
	if(route == NULL)
	{
		return;
	}

	node *node_wpc = route->at("waypoints");
	for(auto name : node_wpc->ls())
	{
		node_wpc->remove(name, true);
	}

	contour c = get_contour();
	//rect r = get_bbox(c);







	yuneec::waypoint_container *wpc = dynamic_cast<yuneec::waypoint_container *>(node_wpc);

	/*yuneec::waypoint *wp_a = wpc->generate("a");
	wp_a->latitude = DEG(proj.y_to_lat(r.top));
	wp_a->longitude = DEG(proj.x_to_lon(r.left));*/
}

void afarea::set_height(double h)
{
	val_height = h;
}

double afarea::get_height() const
{
	return val_height;
}
