#pragma once

#include <vector>
#include <iostream>
#include <map>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "tree/node.h"

#include "geo_node.h"

class route : public node::listener_t, public resource::new_property_listener, public property_listener
{
	typedef sf::CircleShape marker_t;

	std::map<std::string, size_t>		node_nums;
	std::vector<geo_node>				nodes;
	std::vector<marker_t *>				markers;
	std::map<marker_t *, size_t>		marker_nums;
	std::map<node *, size_t>			nodes_to_markers;
	std::map<size_t, node *>			node_ptrs;
	sf::Texture							t;
	sf::Font							f;

	node					*root;

	sf::RectangleShape		sel;
	marker_t				*sel_sp;

	void					child_added				(node *n);
	void					child_removed			(node *, std::string /*name*/);
	void					new_property			(resource *r, property_base *p);

	projector				*proj;

	void					updated					(property_base *prop);

public:
	/*constructor*/			route			();
	/*destructor*/			~route			();

	void					set_projector	(projector *p);
	void					set_tree		(node *n);
	void					update_nodes	(sf::RenderTarget &t, projector *p);
	void					draw			(sf::RenderTarget &t);

	bool					pressed;

	marker_t				*find			(sf::Vector2f pt);

	sf::Vector2f			prev_pos;

	void					mouse			(sf::Vector2f pt);
	void					mouse_press		(sf::Vector2f pt);
	void					mouse_release	(sf::Vector2f /*pt*/);
};
