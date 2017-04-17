#include "map_view.h"

/*constructor*/ map_view::map_view(QWidget *parent) : QSFMLCanvas(parent)//, gn(&s)
{
	move = false;
	setMouseTracking(true);
	tc.set_tiler(&tg);
	proj.set_scale(100.0);
//	s.set_scale_dependent(true);

	rt.set_projector(&proj);
	ar.set_projector(&proj);

	static bool registered = false;
	if(registered == false)
	{
		qRegisterMetaType<geo_point>("geo_point");
		registered = true;
	}
}

/*destructor*/ map_view::~map_view()
{
	//
}

void map_view::update_view()
{
	calculate_zoom();

	setView(view);

	sf::Vector2f tl = mapPixelToCoords(sf::Vector2i(0, 0), view);
	sf::Vector2f bl = mapPixelToCoords(sf::Vector2i(0, height()), view);
	sf::Vector2f br = mapPixelToCoords(sf::Vector2i(width(), height()), view);
	sf::Vector2f tr = mapPixelToCoords(sf::Vector2i(width(), 0), view);

	double top = std::min(tl.y, std::min(bl.y, std::min(br.y, tr.y)));
	double bottom = std::max(tl.y, std::max(bl.y, std::max(br.y, tr.y)));
	double left = std::min(tl.x, std::min(bl.x, std::min(br.x, tr.x)));
	double right = std::max(tl.x, std::max(bl.x, std::max(br.x, tr.x)));

	geo_point north_west = to_geo(sf::Vector2f(left, top));
	geo_point south_east = to_geo(sf::Vector2f(right, bottom));

	tiles = tg.get_tiles_for(north_west.lon(), south_east.lon(), north_west.lat(), south_east.lat(), zoom);

	rt.update_nodes(*this, &proj);
	ar.update_nodes(*this, &proj);
}

void map_view::mousePressEvent(QMouseEvent *event)
{
	sf::Vector2i pos(event->x(), event->y());
	sf::Vector2f viewpos = mapPixelToCoords(pos, view);
	geo_point gp(proj.y_to_lat(viewpos.y), proj.x_to_lon(viewpos.x));
	emit signal_mouse_press(gp);
	if(event->button() == Qt::MiddleButton)
	{
		move = true;
		mouse_pos = pos;
	}
	if(event->button() == Qt::LeftButton)
	{
		//s.setPosition(viewpos.x/* - s.getGlobalBounds().width / 2.0*/, viewpos.y/* - s.getGlobalBounds().height / 2.0*/);
	//	gn.set_geopoint(to_geo(viewpos));
	}
	rt.mouse_press(viewpos);
	ar.mouse_press(viewpos);
}

void map_view::wheelEvent(QWheelEvent *event)
{
	sf::Vector2i pos(event->x(), event->y());
	sf::Vector2f old_pos = mapPixelToCoords(pos, view);

	if(event->modifiers() & Qt::ControlModifier)
	{
		if(event->delta() > 0.0)
		{
			view.rotate(10);
		}
		else
		{
			view.rotate(-10);
		}
	}
	else
	{
		if(event->delta() > 0.0)
		{
			view.zoom(0.5);
		}
		else
		{
			view.zoom(1.5);
		}
	}

	sf::Vector2f new_pos = mapPixelToCoords(pos, view);
	view.move(old_pos - new_pos);

	int old_zoom = zoom;

	update_view();

	if(old_zoom != zoom)
	{
		emit signal_zoom_level(zoom);
	}

	rt.mouse(old_pos);
	ar.mouse(old_pos);
}

void map_view::resizeEvent(QResizeEvent *)
{
	sf::Vector2f c = view.getCenter();
	float w = view.getSize().x;
	setActive();
	sf::RenderWindow::create(winId());
	view = getView();
	view.setCenter(c);
	view.setSize(w, w / (width() / (float)height()));

	update_view();
}

void map_view::mouseMoveEvent(QMouseEvent *event)
{
	sf::Vector2i pos(event->x(), event->y());
	sf::Vector2f viewpos = mapPixelToCoords(pos, view);
	if(move == true)
	{
		sf::Vector2f l = mapPixelToCoords(mouse_pos, view);

		view.move(l - viewpos);

		mouse_pos.x = event->x();
		mouse_pos.y = event->y();

		update_view();
	}

	rt.mouse(viewpos);
	ar.mouse(viewpos);

	emit signal_mouse_move(to_geo(viewpos));
}

void map_view::mouseReleaseEvent(QMouseEvent *event)
{
	sf::Vector2i pos(event->x(), event->y());
	sf::Vector2f viewpos = mapPixelToCoords(pos, view);
	emit signal_mouse_release(to_geo(viewpos));
	move = false;
	rt.mouse_release(viewpos);
	ar.mouse_release(viewpos);
}

void map_view::OnInit()
{
	texture.loadFromFile("/home/dmitry/yandex_disk/programming/sfml_test/test.bmp");
	sprite = sf::Sprite(texture);

	view = getView();

	// Start the game loop
	move = false;
	sf::Vector2i pos;

	t.loadFromFile("/home/dmitry/yandex_disk/programming/sfml_test/settings2.png");
//	s.setTexture(t, true);
	calculate_zoom();
//	s.setOrigin(64,64);
//	s.set_scale_dependent(true);

//	t_child.loadFromFile("/home/dmitry/yandex_disk/programming/sfml_test/settings.png");
//	s_child.setTexture(t_child, true);
	//s_child.setOrigin(64,64);

//	s.add_child(&s_child);

	view.setCenter(0, 50);

	update_view();
}

void map_view::OnUpdate()
{
	clear();

	for(decltype(tiles.size()) i = 0 ; i < tiles.size() ; i += 1)
	{
		int col = tiles[i].get_col_row().x();
		int row = tiles[i].get_col_row().y();

		sf::Texture *t = tc.get_texture(col, row, zoom);

		sf::VertexArray va(sf::TrianglesFan, 4);

		tile::rect_t r = tiles[i].get_rect();

		va[0].position = to_view(r[0]);
		va[1].position = to_view(r[1]);
		va[2].position = to_view(r[2]);
		va[3].position = to_view(r[3]);

		va[0].texCoords = sf::Vector2f(0, 0);
		va[1].texCoords = sf::Vector2f(256, 0);
		va[2].texCoords = sf::Vector2f(256, 256);
		va[3].texCoords = sf::Vector2f(0, 256);

		sf::RenderStates rs;
		rs.texture = t;
		draw(va, rs);
	}

	/*s_child.rotate(0.1);

	gn.map(proj);
	s.render(*this);*/

	rt.draw(*this);
	ar.draw(*this);

	display();
}

int map_view::calculate_zoom()
{
	double w = view.getSize().x / proj.get_scale();
	double resolution = width() / w;
	double tile_count = resolution / tg.get_base_tile_size();
	tile_count = ceil(tile_count);
	double z = log2(tile_count);

	//qDebug() << tile_count << z << w;

	zoom = ceil(z);
	zoom = zoom < 0 ? 0 : zoom > 20 ? 20 : zoom;

	return zoom;
}

geo_point map_view::to_geo(const sf::Vector2f &view_coord) const
{
	return geo_point(proj.y_to_lat(view_coord.y), proj.x_to_lon(view_coord.x));
}

sf::Vector2f map_view::to_view(const geo_point &gp) const
{
	return sf::Vector2f(proj.lon_to_x(gp.lon()), proj.lat_to_y(gp.lat()));
}
