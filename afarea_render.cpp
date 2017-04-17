#include "afarea_render.h"

#define RAD(x) (x / 180.0 * M_PI)
#define DEG(x) (x * 180.0 / M_PI)

/*constructor*/			afarea_render::afarea_render			()
{
	root = NULL;
	sel_sp = NULL;

	sel.setFillColor(sf::Color::Transparent);
	sel.setOutlineColor(sf::Color::Red);

	pressed = false;

	proj = NULL;
}

/*destructor*/			afarea_render::~afarea_render			()
{
	//
}

void				afarea_render::child_added				(node *n)
{
	std::string path = n->get_parent()->get_path();
	if(path != "/route/afarea")
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

void				afarea_render::child_removed			(node *n, std::string name)
{
	std::cout << "removed" << n->get_path() << " " << name << std::endl;

	const std::string &path = n->get_path();
	if(path != "/route/afarea")
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
}

void				afarea_render::new_property			(resource *r, property_base *p)
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

void					afarea_render::mouse_press		(sf::Vector2f pt)
{
	marker_t *new_found = find(pt);
	if(new_found != NULL)
	{
		printf("\n\n\nfound\n\n\n");
		sel_sp = new_found;
		prev_pos = pt;
		pressed = true;
	}
}

void					afarea_render::mouse_release	(sf::Vector2f /*pt*/)
{
	pressed = false;
	sel_sp = NULL;
}

void					afarea_render::mouse			(sf::Vector2f pt)
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
		sf::Vector2f shift = pt - prev_pos;
		printf("shift is %f %f\n", shift.x, shift.y);
		sel_sp->move(shift);

		sf::Vector2f pos = sel_sp->getPosition();
		printf("pos is %f %f\n", pos.x, pos.y);
		geo_point gp(proj->y_to_lat(pos.y), proj->x_to_lon(pos.x));
		printf("gp is %f %f\n", gp.lat(), gp.lon());
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

afarea_render::marker_t *afarea_render::find			(sf::Vector2f pt)
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

void					afarea_render::set_projector	(projector *p)
{
	proj = p;
}

void					afarea_render::set_tree		(node *n)
{
	n->add_listener(this, true);
	root = n;
}

void					afarea_render::update_nodes	(sf::RenderTarget &t, projector *p)
{
	for(auto node : nodes)
	{
		node.map(*p);
		double sc = t.getView().getSize().x / t.getSize().x / 4.0;
		node.get()->setScale(sc, sc);
	}
}

void					afarea_render::draw			(sf::RenderTarget &t)
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
	if(va.getVertexCount() > 2)
	{
		va.append(va[0]);
	}

	t.draw(va);

	for(auto sp : markers)
	{
		t.draw(*sp);

		std::stringstream ss;
		ss << marker_nums[sp];

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

void afarea_render::updated(property_base *prop)
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
