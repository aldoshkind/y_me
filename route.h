#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <iostream>
#include <map>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "tree/node.h"

#include "geo_node.h"

#define RAD(x) (x / 180.0 * M_PI)

class route : public node::listener_t, public resource::new_property_listener
{
	std::map<std::string, size_t>		node_nums;
	std::vector<geo_node>				nodes;
	std::vector<sf::Sprite *>			sprites;
	std::map<sf::Sprite *, size_t>		sprite_nums;
	std::map<size_t, node *>			node_ptrs;
	sf::Texture							t;
	sf::Font							f;

	node						*root;

	sf::RectangleShape			sel;
	sf::Sprite					*sel_sp;

	void				child_added				(node *n)
	{
		std::string path = n->get_parent()->get_path();
		if(path != "/route/waypoints")
		{
			return;
		}

		n->add_listener(this);
		n->resource::add_listener(this);

		node_nums[n->get_name()] = nodes.size();

		sf::Sprite *spr = NULL;
		sprites.push_back(spr = new sf::Sprite(t));

		sprite_nums[spr] = nodes.size();
		node_ptrs[nodes.size()] = n;

		spr->setOrigin(spr->getLocalBounds().width / 2.0, spr->getLocalBounds().height / 2.0);
		nodes.push_back(geo_node(sprites.back()));
		nodes.back().set_geopoint(geo_point(RAD(56), RAD(92)));
	}

	void				child_removed			(node *, std::string /*name*/)
	{
		//
	}

	void				new_property			(resource *r, property_base *p)
	{
		std::cout << p->get_name() << std::endl;

		node *n = dynamic_cast<node *>(r);
		property<double> *pd = dynamic_cast<property<double> *>(p);
		if(n == NULL || pd == NULL)
		{
			return;
		}

		if(node_nums.find(n->get_name()) == node_nums.end())
		{
			return;
		}

		if(pd->get_name() == "latitude")
		{
			nodes[node_nums[n->get_name()]].get_geopoint().set_lat(RAD(pd->get_value()));
		}
		else if(pd->get_name() == "longitude")
		{
			nodes[node_nums[n->get_name()]].get_geopoint().set_lon(RAD(pd->get_value()));
		}
	}

	projector				*proj;

public:
	/*constructor*/			route			()
	{
		t.loadFromFile("/home/dmitry/dot.png");
		root = NULL;
		sel_sp = NULL;
		f.loadFromFile("/home/dmitry/antiqua.ttf");

		sel.setFillColor(sf::Color::Transparent);
		sel.setOutlineColor(sf::Color::Red);

		pressed = false;

		proj = NULL;
	}

	/*destructor*/			~route			()
	{
		//
	}

	void					set_projector	(projector *p)
	{
		proj = p;
	}

	void					set_tree		(node *n)
	{
		n->add_listener(this, true);
		root = n;
	}

	void					update_nodes	(sf::RenderTarget &t, projector *p)
	{
		for(auto node : nodes)
		{
			node.map(*p);
			double sc = t.getView().getSize().x / t.getSize().x / 4.0;
			node.get()->setScale(sc, sc);
		}
	}

	void					draw			(sf::RenderTarget &t)
	{
		update_nodes(t, proj);

		sf::VertexArray va(sf::LinesStrip);

		for(auto sp : sprites)
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

		for(auto sp : sprites)
		{
			t.draw(*sp);

			std::stringstream ss;
			ss << sprite_nums[sp];

			sf::Text tx(ss.str(), f, 28);
			double sc = t.getView().getSize().x / t.getSize().x;
			tx.setOrigin(tx.getLocalBounds().width / 2.0, tx.getLocalBounds().height / 2.0);
			tx.setScale(sc, sc);
			tx.setPosition(sp->getPosition());
			t.draw(tx);
		}
		if(sel_sp != NULL)
		{
			t.draw(sel);
		}
	}

	bool					pressed;

	sf::Sprite				*find			(sf::Vector2f pt)
	{
		for(auto sp : sprites)
		{
			sf::FloatRect fr = sp->getGlobalBounds();
			if(fr.contains(pt))
			{
				return sp;
			}
		}
		return NULL;
	}

	sf::Vector2f			prev_pos;

	void					mouse			(sf::Vector2f pt)
	{
		if(pressed == false)
		{
			sf::Sprite *new_found = find(pt);

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
			nodes[sprite_nums[sel_sp]].set_geopoint(gp);
			prev_pos = pt;

			node *n = node_ptrs[sprite_nums[sel_sp]];
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

	void					mouse_press		(sf::Vector2f pt)
	{
		sf::Sprite *new_found = find(pt);
		if(new_found != NULL)
		{
			sel_sp = new_found;
			prev_pos = pt;
			pressed = true;
		}
	}

	void					mouse_release	(sf::Vector2f /*pt*/)
	{
		pressed = false;
	}
};

#endif // ROUTE_H
