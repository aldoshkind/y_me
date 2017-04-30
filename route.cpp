#include "route.h"

#define RAD(x) (x / 180.0 * M_PI)
#define DEG(x) (x * 180.0 / M_PI)

/*constructor*/			route::route			()
{
	root = NULL;
	sel_sp = NULL;
	f.loadFromFile("/home/dmitry/antiqua.ttf");

	sel.setFillColor(sf::Color::Transparent);
	sel.setOutlineColor(sf::Color::Red);

	pressed = false;

	proj = NULL;
}

/*destructor*/			route::~route			()
{
	//
}

void				route::child_added				(node *n)
{
	std::string path = n->get_parent()->get_path();
	if(path != "/route/waypoints")
	{
		return;
	}
	printf("new child %s\n", n->get_name().c_str());

	node_nums[n->get_name()] = nodes.size();

	sf::CircleShape *cs = new sf::CircleShape(30, 28);
	nodes_to_markers[n] = markers.size();
	markers.push_back(cs);

	marker_nums[cs] = nodes.size();
	node_ptrs[nodes.size()] = n;

	cs->setOrigin(cs->getLocalBounds().width / 2.0, cs->getLocalBounds().height / 2.0);
	nodes.push_back(geo_node(markers.back()));
	nodes.back().set_geopoint(geo_point(RAD(56), RAD(92)));

	n->add_listener(this);
	n->resource::add_listener(this);
}

void				route::child_removed			(node *n, std::string name)
{
	std::cout << "removed" << n->get_path() << " " << name << std::endl;

	const std::string &path = n->get_path();
	if(path != "/route/waypoints")
	{
		return;
	}

	if(node_nums.find(name) == node_nums.end())
	{
		return;
	}

	size_t id = node_nums[name];
	node_ptrs.erase(id);
	marker_nums.erase(markers[id]);
	nodes.erase(nodes.begin() + id);
	markers.erase(markers.begin() + id);
	node_nums.erase(name);

	for(std::map<std::string, size_t>::iterator it = node_nums.begin() ; it != node_nums.end() ; ++it)
	{
		if(it->second > id)
		{
			--it->second;
		}
	}


	//std::map<std::string, size_t>		node_nums;
	//std::vector<geo_node>				nodes;
	//std::vector<marker_t *>				markers;
	//std::map<marker_t *, size_t>		marker_nums;
	//std::map<size_t, node *>			node_ptrs;
}

void				route::new_property			(resource *r, property_base *p)
{
	//std::cout << p->get_name() << std::endl;

	node *n = dynamic_cast<node *>(r);
	property<double> *pd = dynamic_cast<property<double> *>(p);
	if(n == NULL || pd == NULL)
	{
		return;
	}

	printf("new prop %s of %s\n", p->get_name().c_str(), n->get_name().c_str());

	if(node_nums.find(n->get_name()) == node_nums.end())
	{
		return;
	}

	if(pd->get_name() == "latitude")
	{
		nodes[node_nums[n->get_name()]].get_geopoint().set_lat(RAD(pd->get_value()));
		pd->add_listener(this);
	}
	else if(pd->get_name() == "longitude")
	{
		nodes[node_nums[n->get_name()]].get_geopoint().set_lon(RAD(pd->get_value()));
		pd->add_listener(this);
	}
}

void					route::mouse_press		(sf::Vector2f pt)
{
	marker_t *new_found = find(pt);
	if(new_found != NULL)
	{
		sel_sp = new_found;
		prev_pos = pt;
		pressed = true;
	}
}

void					route::mouse_release	(sf::Vector2f /*pt*/)
{
	pressed = false;
}

void					route::mouse			(sf::Vector2f pt)
{
	if(pressed == false)
	{
		marker_t *new_found = find(pt);

		if(new_found != NULL)
		{
			sf::FloatRect fr = new_found->getGlobalBounds();
			sel.setSize(sf::Vector2f(fr.width, fr.height));
			sel.setOutlineThickness(fr.width * 0.05);
			sel.setPosition(fr.left, fr.top);
		}

		sel_sp = new_found;
	}
	else
	{
		sel_sp->move(pt - prev_pos);

		sf::Vector2f pos = sel_sp->getPosition();
		geo_point gp(proj->y_to_lat(pos.y), proj->x_to_lon(pos.x));
		nodes[marker_nums[sel_sp]].set_geopoint(gp);
		prev_pos = pt;

		node *n = node_ptrs[marker_nums[sel_sp]];
		resource::props_t props = n->get_properties();

		for(auto prop : props)
		{
			if(prop->get_name() != "latitude" && prop->get_name() != "longitude")
			{
				continue;
			}

			property<double> *pr = dynamic_cast<property<double> *>(prop);
			if(pr == NULL)
			{
				continue;
			}
			if(pr->get_name() == "latitude")
			{
				*pr = DEG(gp.lat());
			}
			else
			{
				*pr = DEG(gp.lon());
			}
		}

		if(sel_sp != NULL)
		{
			sf::FloatRect fr = sel_sp->getGlobalBounds();
			sel.setSize(sf::Vector2f(fr.width, fr.height));
			sel.setOutlineThickness(fr.width * 0.05);
			sel.setPosition(fr.left, fr.top);
		}
	}
}

route::marker_t *route::find			(sf::Vector2f pt)
{
	for(auto sp : markers)
	{
		sf::FloatRect fr = sp->getGlobalBounds();
		if(fr.contains(pt))
		{
			return sp;
		}
	}
	return NULL;
}

void					route::set_projector	(projector *p)
{
	proj = p;
}

void					route::set_tree		(node *n)
{
	n->add_listener(this, true);
	root = n;
}

void					route::update_nodes	(sf::RenderTarget &t, projector *p)
{
	for(auto node : nodes)
	{
		node.map(*p);
		double sc = t.getView().getSize().x / t.getSize().x / 4.0;
		node.get()->setScale(sc, sc);
	}
}

void					route::draw			(sf::RenderTarget &t)
{
	update_nodes(t, proj);

	sf::VertexArray va(sf::LinesStrip);

	for(auto sp : markers)
	{
		sf::Vertex v;
		v.position = sp->getPosition();
		if(sp == sel_sp)
		{
			v.color = sf::Color::Red;
		}
		va.append(v);
	}

	t.draw(va);

	for(auto sp : markers)
	{
		t.draw(*sp);

		std::stringstream ss;
		ss << marker_nums[sp];

		//node_ptrs[marker_nums[sp]]->get_name()

		sf::Text tx(node_ptrs[marker_nums[sp]]->get_name(), f, 28);
		double sc = t.getView().getSize().x / t.getSize().x;
		tx.setOrigin(tx.getLocalBounds().width / 2.0, tx.getLocalBounds().height / 2.0);
		tx.setScale(sc, sc);
		tx.setPosition(sp->getPosition());
		tx.setRotation(t.getView().getRotation());
		t.draw(tx);
	}
	if(sel_sp != NULL)
	{
		t.draw(sel);
	}
}

void route::updated(property_base *prop)
{
	node *n = dynamic_cast<node *>(prop->get_resource());

	if(n == NULL)
	{
		return;
	}

	if(nodes_to_markers.find(n) == nodes_to_markers.end())
	{
		return;
	}

	property<double> *pd = dynamic_cast<property<double> *>(prop);

	if(pd == NULL)
	{
		return;
	}

	size_t i = nodes_to_markers[n];

	if(prop->get_name() == "latitude")
	{
		nodes[i].get_geopoint().set_lat(RAD(pd->get_value()));
	}
	if(prop->get_name() == "longitude")
	{
		nodes[i].get_geopoint().set_lon(RAD(pd->get_value()));
	}
}
