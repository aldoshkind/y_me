#pragma once

#include <QMouseEvent>

#include "QSFMLCanvas.h"

#include "texture_cache.h"
#include "projector_spheric.h"
#include "tiler_google.h"
#include "additional_math.h"
#include "render_node.h"
#include "geo_node.h"
#include "route.h"
#include "afarea_render.h"

Q_DECLARE_METATYPE(geo_point)

class map_view : public QSFMLCanvas
{
	Q_OBJECT

	sf::Sprite			sprite;
	sf::View			view;
	sf::Texture			texture;
	sf::Texture			t;

	/*geo_node			gn;
	sprite_node			s;
	sf::Texture			t_child;
	sprite_node			s_child;*/

	tiler_google		tg;

	bool				move;
	sf::Vector2i		mouse_pos;

	int					zoom;

	tiler::tiles_t		tiles;
	texture_cache		tc;
	projector_spheric	proj;

	sf::VertexArray		view_rect;

	route				rt;
	afarea_render		ar;

	geo_point			to_geo				(const sf::Vector2f &view_coord) const;
	sf::Vector2f		to_view				(const geo_point &gp) const;

	void				update_view			();
	void				mousePressEvent		(QMouseEvent *event);
	void				wheelEvent			(QWheelEvent *event);
	void				resizeEvent			(QResizeEvent *);
	void				mouseMoveEvent		(QMouseEvent *event);
	void				mouseReleaseEvent	(QMouseEvent *);

public:
	/*constructor*/		map_view			(QWidget *parent = NULL);
	/*destructor*/		~map_view			();

	void				OnInit				();
	void				OnUpdate			();

	int					calculate_zoom		();

	route				&get_route			()
	{
		return rt;
	}

	afarea_render		&get_afarea_render	()
	{
		return ar;
	}

signals:
	void				signal_mouse_move	(geo_point gp);
	void				signal_zoom_level	(int zoom);
	void				signal_mouse_press	(geo_point gp);
	void				signal_mouse_release(geo_point gp);
};
