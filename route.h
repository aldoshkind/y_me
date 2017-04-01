#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <iostream>
#include <map>

#include <SFML/Graphics.hpp>

#include "tree/node.h"

#include "geo_node.h"

#define RAD(x) (x / 180.0 * M_PI)

class route : public node::listener_t, public resource::new_property_listener
{
	std::map<std::string, size_t>		node_nums;
	std::vector<geo_node>				nodes;
	std::vector<sf::Sprite *>	sprites;
	sf::Texture					t;

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
		spr->setOrigin(spr->getLocalBounds().width / 2.0, spr->getLocalBounds().height / 2.0);
		spr->setScale(0.005, 0.005);
		nodes.push_back(geo_node(sprites.back()));
		nodes.back().set_geopoint(geo_point(RAD(56), RAD(92)));
	}

	void				child_removed			(node *, std::string name)
	{
		//
	}

	void				new_property			(resource *r, property_base *p)
	{
		std::cout << p->get_name() << endl;

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

public:
	/*constructor*/			route			()
	{
		t.loadFromFile("/home/dmitry/dot.png");
		root = NULL;
		sel_sp = NULL;

		sel.setFillColor(sf::Color::Transparent);
		sel.setOutlineColor(sf::Color::Red);
	}

	/*destructor*/			~route			()
	{
		//
	}

	void					set_tree		(node *n)
	{
		n->add_listener(this, true);
		root = n;
	}

	void					draw			(sf::RenderTarget &t, projector *p)
	{
		for(auto node : nodes)
		{
			node.map(*p);
			double sc = t.getView().getSize().x / t.getSize().x / 4.0;
			node.get()->setScale(sc, sc);
		}
		for(auto sp : sprites)
		{
			t.draw(*sp);
		}
		if(sel_sp != NULL)
		{
			t.draw(sel);
		}
	}

	void					mouse			(sf::Vector2f pt)
	{
		sel_sp = NULL;
		for(auto sp : sprites)
		{
			sf::Sprite *ss;
			sf::FloatRect fr = sp->getGlobalBounds();
			if(fr.contains(pt))
			{
				if(sel_sp != sp)
				{
					sel.setSize(sf::Vector2f(fr.width, fr.height));
					sel.setOutlineThickness(fr.width * 0.05);
					sel.setPosition(fr.left, fr.top);
				}
				sel_sp = sp;
				break;
			}
		}
	}
};

#endif // ROUTE_H
