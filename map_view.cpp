#include "map_view.h"

#define SCALE_COEF 1000.0

/*constructor*/ map_view::map_view(QWidget *parent) : QSFMLCanvas(parent)
{
	move = false;
	setMouseTracking(true);
	tc.set_tiler(&tg);

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

	sf::Vector2f tl = mapPixelToCoords(sf::Vector2i(0, 0), view);
	sf::Vector2f bl = mapPixelToCoords(sf::Vector2i(0, height()), view);
	sf::Vector2f br = mapPixelToCoords(sf::Vector2i(width(), height()), view);
	sf::Vector2f tr = mapPixelToCoords(sf::Vector2i(width(), 0), view);

	double top = std::min(tl.y, std::min(bl.y, std::min(br.y, tr.y)));
	double bottom = std::max(tl.y, std::max(bl.y, std::max(br.y, tr.y)));
	double left = std::min(tl.x, std::min(bl.x, std::min(br.x, tr.x)));
	double right = std::max(tl.x, std::max(bl.x, std::max(br.x, tr.x)));

	double lat_top = proj.y_to_lat(top / SCALE_COEF);
	double lat_bottom = proj.y_to_lat(bottom / SCALE_COEF);
	double lon_left = proj.x_to_lon(left / SCALE_COEF);
	double lon_right = proj.x_to_lon(right / SCALE_COEF);

	tiles = tg.get_tiles_for(lon_left, lon_right, lat_top, lat_bottom, zoom);
}

void map_view::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::MiddleButton)
	{
		move = true;
		mouse_last_pos.x = event->x();
		mouse_last_pos.y = event->y();
	}
	if(event->button() == Qt::LeftButton)
	{
		sf::Vector2i pi;
		pi.x = event->x();
		pi.y = event->y();

		sf::Vector2f p = mapPixelToCoords(pi, view);
		s.setPosition(p.x - s.getGlobalBounds().width / 2.0, p.y - s.getGlobalBounds().height / 2.0);
	}
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
	sf::Vector2f c = mapPixelToCoords(sf::Vector2i(event->x(), event->y()), view);
	if(move == true)
	{
		sf::Vector2f l = mapPixelToCoords(mouse_last_pos, view);

		view.move(l - c);

		mouse_last_pos.x = event->x();
		mouse_last_pos.y = event->y();

		update_view();
	}

	emit signal_mouse_move(geo_point(proj.y_to_lat(c.y / SCALE_COEF), proj.x_to_lon(c.x / SCALE_COEF)));
}

void map_view::mouseReleaseEvent(QMouseEvent *)
{
	move = false;
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
	s = sf::Sprite(t);
	calculate_zoom();

	view.setCenter(0, 50);

	update_view();
}

void map_view::OnUpdate()
{
	setView(view);
	clear();

	for(decltype(tiles.size()) i = 0 ; i < tiles.size() ; i += 1)
	{
		int col = tiles[i].get_col_row().x();
		int row = tiles[i].get_col_row().y();

		sf::Texture *t = tc.get_texture(col, row, zoom);

		sf::VertexArray va(sf::TrianglesFan, 4);

		tile::rect_t r = tiles[i].get_rect();

		va[0].position = sf::Vector2f(proj.lon_to_x(r[0].lon()) * SCALE_COEF, proj.lat_to_y(r[0].lat()) * SCALE_COEF);
		va[1].position = sf::Vector2f(proj.lon_to_x(r[1].lon()) * SCALE_COEF, proj.lat_to_y(r[1].lat()) * SCALE_COEF);
		va[2].position = sf::Vector2f(proj.lon_to_x(r[2].lon()) * SCALE_COEF, proj.lat_to_y(r[2].lat()) * SCALE_COEF);
		va[3].position = sf::Vector2f(proj.lon_to_x(r[3].lon()) * SCALE_COEF, proj.lat_to_y(r[3].lat()) * SCALE_COEF);

		va[0].texCoords = sf::Vector2f(0, 0);
		va[1].texCoords = sf::Vector2f(256, 0);
		va[2].texCoords = sf::Vector2f(256, 256);
		va[3].texCoords = sf::Vector2f(0, 256);

		sf::RenderStates rs;
		rs.texture = t;
		draw(va, rs);
		//draw(&va[0], 4, sf::LinesStrip);
	}

	display();
}

int map_view::calculate_zoom()
{
	double w = view.getSize().x / SCALE_COEF;
	double resolution = width() / w;
	double tile_count = resolution / tg.get_base_tile_size();
	tile_count = ceil(tile_count);
	double z = log2(tile_count);

	qDebug() << tile_count << z << w;

	zoom = ceil(z);
	zoom = zoom < 0 ? 0 : zoom > 20 ? 20 : zoom;

	return zoom;
}
