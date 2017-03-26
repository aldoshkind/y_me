#pragma once

#include <QMouseEvent>

#include "QSFMLCanvas.h"

#include "texture_cache.h"
#include "projector_spheric.h"
#include "tiler_google.h"
#include "additional_math.h"

Q_DECLARE_METATYPE(geo_point)

class map_view : public QSFMLCanvas
{
	Q_OBJECT

	sf::Sprite			sprite;
	sf::View			view;
	sf::Texture			texture;
	sf::Texture			t;
	sf::Sprite			s;

	tiler_google		tg;

	bool				move;
	sf::Vector2i		mouse_last_pos;

	int					zoom;

	tiler::tiles_t		tiles;
	texture_cache		tc;
	projector_spheric	proj;

	sf::VertexArray		view_rect;

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

signals:
	void				signal_mouse_move	(geo_point gp);
	void				signal_zoom_level	(int zoom);
};
