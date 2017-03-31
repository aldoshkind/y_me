#ifndef RENDER_NODE_H
#define RENDER_NODE_H

#include <vector>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

class render_node
{
public:
	/*constructor*/					render_node				()
	{
		//
	}

	virtual /*destructor*/			~render_node			()
	{
		//
	}

	//virtual sf::Transform			get_transform			() const = 0;

	void							render					(sf::RenderTarget &target)
	{
		render(target, sf::Transform());
	}

	void							render					(sf::RenderTarget &target, const sf::Transform &parentTransform)
	{
		onDraw(target, parentTransform);

		if(children.size() != 0)
		{
			const sf::Transformable *transformable = dynamic_cast<const sf::Transformable *>(this);
			sf::Transform tr;
			if(transformable != NULL)
			{
				tr = transformable->getTransform();
				tr.translate(transformable->getOrigin());
			}

			sf::Transform combinedTransform = parentTransform * tr;
			for(std::size_t i = 0; i < children.size(); ++i)
			{
				children[i]->render(target, combinedTransform);
			}
		}
	}

	void							remove_child			(render_node *n)
	{
		children_t::iterator found = std::find(children.begin(), children.end(), n);
		if(found != children.end())
		{
			children.erase(found);
		}
	}

	void							add_child				(render_node *n)
	{
		remove_child(n);
		children.push_back(n);
	}

private:

	virtual void						onDraw						(sf::RenderTarget &target, const sf::Transform &parent_transform) = 0;

	sf::Transform						m_transform;
	typedef std::vector<render_node *>	children_t;
	children_t							children;
};

class sprite_node : public sf::Sprite, public render_node
{
	bool							scale_independent;
public:
	/*constructor*/					sprite_node				()
	{
		scale_independent = false;
	}

	virtual /*destructor*/			~sprite_node			()
	{
		//
	}

	void							set_scale_dependent		(bool sd)
	{
		scale_independent = sd;
	}

	void							onDraw					(sf::RenderTarget &target, const sf::Transform &parent_transform)
	{
		sf::Vector2f scale;
		if(scale_independent == true)
		{
			scale = getScale();
			double sc = target.getView().getSize().x / target.getSize().x;
			setScale(sc, sc);
		}

		target.draw(*this, parent_transform);
		if(scale_independent == true)
		{
			setScale(scale);
		}
	}
};


#endif // RENDER_NODE_H
