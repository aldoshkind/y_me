#include "afarea.h"

#include "lib2dgeom/line2.h"

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

	if(idx > 4)
	{
		return;
	}

	printf("created\n");

	node *n = append("vx" + ss.str());
	auto lat = new property_value<double>("latitude");
	auto lon = new property_value<double>("longitude");
	n->add_property(lat);
	n->add_property(lon);
	*lat = DEG(pos.lat());
	*lon = DEG(pos.lon());

	lat->add_listener(this);
	lon->add_listener(this);

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
	rect r = get_bbox(c);

	yuneec::waypoint_container *wpc = dynamic_cast<yuneec::waypoint_container *>(node_wpc);

	double step = 0.01;
	std::map<double, std::pair<vector2d, vector2d> > profiles;

	bool upside = true;
	for(double x = r.left + step / 2.0 ; x < r.left + r.width ; x += step)
	{
		line2 l(x, 0);
		for(contour::size_type i = 0 ; i < c.size() ; i += 1)
		{
			vector2d a = vector2d(c[i].x, c[i].y);
			vector2d b = vector2d(c[(i + 1) % c.size()].x, c[(i + 1) % c.size()].y);
			line2 l0(a, b);
			vector2d inter = l.intersection(l0);
			if(within(inter.x, a.x, b.x) == false)
			{
				continue;
			}
			if(profiles.find(x) == profiles.end())
			{
				profiles[x].first = profiles[x].second = inter;
			}
			else
			{
				if(profiles[x].first.y < inter.y)
				{
					profiles[x].first = inter;
				}
				if(profiles[x].second.y > inter.y)
				{
					profiles[x].second = inter;
				}
			}
		}

		if(profiles.find(x) == profiles.end())
		{
			continue;
		}

		std::stringstream ss;
		ss << x;

		if(upside == true)
		{
			yuneec::waypoint *wp_a = wpc->generate("a" + ss.str());
			wp_a->latitude = DEG(proj.y_to_lat(profiles[x].first.y));
			wp_a->longitude = DEG(proj.x_to_lon(profiles[x].first.x));
		}

		yuneec::waypoint *wp_b = wpc->generate("b" + ss.str());
		wp_b->latitude = DEG(proj.y_to_lat(profiles[x].second.y));
		wp_b->longitude = DEG(proj.x_to_lon(profiles[x].second.x));

		if(upside == false)
		{
			yuneec::waypoint *wp_a = wpc->generate("a" + ss.str());
			wp_a->latitude = DEG(proj.y_to_lat(profiles[x].first.y));
			wp_a->longitude = DEG(proj.x_to_lon(profiles[x].first.x));
		}

		upside = !upside;
	}



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

void afarea::updated(property_base *prop)
{
	if(ls().size() > 2)
	{
		update_route();
	}
}
