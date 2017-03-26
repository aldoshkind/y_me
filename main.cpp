#include <queue>

#include <unistd.h>

#include "QSFMLCanvas.h"
#include <QApplication>

#include <GL/gl.h>

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QWheelEvent>
#include <QPointF>
#include <QtConcurrent/QtConcurrent>

#include <curl/curl.h>

#include "tiler_google.h"
#include "texture_cache.h"
#include "projector_spheric.h"

#define RAD(x) (x / 180.0 * M_PI)

template <class value_type>
inline value_type clamp(const value_type &v, const value_type &lower, const value_type &upper)
{
	return v > upper ? upper : (v < lower ? lower : v);
}

class sfml_rel : public QSFMLCanvas
{
	sf::Sprite			sprite;
	sf::View			view;
	sf::Texture			texture;
	sf::Texture			t;
	sf::Sprite			s;

	tiler_google		tg;

	bool				move;
	sf::Vector2i		mouse_last_pos;

	int					last_zoom;
	sf::Font			font;
	tiler::tiles_t		tiles;

	texture_cache		tc;

	projector_spheric	proj;

	sf::VertexArray		view_rect;

	void				update_view			()
	{
		calculate_zoom();

		sf::Vector2f c = view.getCenter();

		sf::Vector2f tl = mapPixelToCoords(sf::Vector2i(0, 0), view);
		sf::Vector2f bl = mapPixelToCoords(sf::Vector2i(0, height()), view);
		sf::Vector2f br = mapPixelToCoords(sf::Vector2i(width(), height()), view);
		sf::Vector2f tr = mapPixelToCoords(sf::Vector2i(width(), 0), view);

		double top = std::min(tl.y, std::min(bl.y, std::min(br.y, tr.y)));
		double bottom = std::max(tl.y, std::max(bl.y, std::max(br.y, tr.y)));
		double left = std::min(tl.x, std::min(bl.x, std::min(br.x, tr.x)));
		double right = std::max(tl.x, std::max(bl.x, std::max(br.x, tr.x)));

		double lat_top = proj.y_to_lat(top / 100);
		double lat_bottom = proj.y_to_lat(bottom / 100);
		double lon_left = proj.x_to_lon(left / 100);
		double lon_right = proj.x_to_lon(right / 100);

		view_rect.clear();

		view_rect.append(sf::Vertex(sf::Vector2f(lon_left, clamp(lat_top, -1.48, 1.48))));
		view_rect.append(sf::Vertex(sf::Vector2f(lon_right, clamp(lat_top, -1.48, 1.48))));
		view_rect.append(sf::Vertex(sf::Vector2f(lon_right, clamp(lat_bottom, -1.48, 1.48))));
		view_rect.append(sf::Vertex(sf::Vector2f(lon_left, clamp(lat_bottom, -1.48, 1.48))));
		view_rect.append(sf::Vertex(sf::Vector2f(lon_left, clamp(lat_top, -1.48, 1.48))));

		tiles = tg.get_tiles_for(lon_left, lon_right, lat_top, lat_bottom, last_zoom);
	}

	void				mousePressEvent		(QMouseEvent *event)
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

	void				wheelEvent			(QWheelEvent *event)
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

		update_view();
	}

	void				resizeEvent			(QResizeEvent *)
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

	void				mouseMoveEvent		(QMouseEvent *event)
	{
		if(move == true)
		{
			sf::Vector2f l = mapPixelToCoords(mouse_last_pos, view);
			sf::Vector2f c = mapPixelToCoords(sf::Vector2i(event->x(), event->y()), view);

			view.move(l - c);

			mouse_last_pos.x = event->x();
			mouse_last_pos.y = event->y();

			update_view();
		}
	}

	void				mouseReleaseEvent	(QMouseEvent *)
	{
		move = false;
	}

public:
	/*constructor*/		sfml_rel			(QWidget *parent = NULL) : QSFMLCanvas(parent)
	{
		move = false;
		setMouseTracking(true);
		tc.set_tiler(&tg);
	}
	/*destructor*/		~sfml_rel			()
	{
		//
	}

	void				OnInit				()
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

		font.loadFromFile("/home/dmitry/yandex_disk/programming/sfml_test/antiqua.ttf");

		view.setCenter(0, 50);

		update_view();
	}

	void				OnUpdate			()
	{
		setView(view);
		clear();

		for(int i = 0 ; i < tiles.size() ; i += 1)
		{
			int col = tiles[i].get_col_row().x();
			int row = tiles[i].get_col_row().y();

			sf::Texture *t = tc.get_texture(col, row, last_zoom);

			sf::VertexArray va(sf::TrianglesFan, 4);

			tile::rect_t r = tiles[i].get_rect();

			double x = 1 - proj.lon_to_x(r[1].longitude());
			double y = 1 - proj.lat_to_y(r[1].latitude());

			va[0].position = sf::Vector2f(proj.lon_to_x(r[0].longitude()) * 100, proj.lat_to_y(r[0].latitude()) * 100);
			va[1].position = sf::Vector2f(proj.lon_to_x(r[1].longitude()) * 100, proj.lat_to_y(r[1].latitude()) * 100);
			va[2].position = sf::Vector2f(proj.lon_to_x(r[2].longitude()) * 100, proj.lat_to_y(r[2].latitude()) * 100);
			va[3].position = sf::Vector2f(proj.lon_to_x(r[3].longitude()) * 100, proj.lat_to_y(r[3].latitude()) * 100);

			va[0].texCoords = sf::Vector2f(0, 0);
			va[1].texCoords = sf::Vector2f(256, 0);
			va[2].texCoords = sf::Vector2f(256, 256);
			va[3].texCoords = sf::Vector2f(0, 256);

			sf::RenderStates rs;
			rs.texture = t;
			draw(va, rs);
			draw(&va[0], 4, sf::LinesStrip);
		}

		sf::VertexArray va;

		va.append(sf::Vector2f(proj.lon_to_x(view_rect[0].position.x) + 10, proj.lat_to_y(view_rect[0].position.y) * 100));
		va.append(sf::Vector2f(proj.lon_to_x(view_rect[1].position.x) - 10, proj.lat_to_y(view_rect[1].position.y) * 100));
		va.append(sf::Vector2f(proj.lon_to_x(view_rect[2].position.x) - 10, proj.lat_to_y(view_rect[2].position.y) * 100));
		va.append(sf::Vector2f(proj.lon_to_x(view_rect[3].position.x) + 10, proj.lat_to_y(view_rect[3].position.y) * 100));
		va.append(sf::Vector2f(proj.lon_to_x(view_rect[4].position.x) + 10, proj.lat_to_y(view_rect[4].position.y) * 100));

		draw(&va[0],  5, sf::LinesStrip);


		display();
	}

	int calculate_zoom()
	{
		double deg_per_px = view.getSize().x / width();
		double tiles_count = (360.0) / ((deg_per_px) * tg.get_base_tile_size());
		double zoom = log2(tiles_count);

		last_zoom = ceil(zoom);

		last_zoom = last_zoom < 0 ? 0 : last_zoom > 20 ? 20 : last_zoom;

		//last_zoom = last_zoom > 10 ? 10 : last_zoom;

		//qDebug() << "zoom is " << zoom << last_zoom;

		return last_zoom;
	}
};

class main_win : public QWidget
{
	QVBoxLayout			*layout_main;
	sfml_rel			*sfml;

public:
	/*constructor*/		main_win			() : QWidget(NULL)
	{
		layout_main = new QVBoxLayout(this);
		sfml = new sfml_rel(this);
		layout_main->addWidget(sfml);
	}

	/*destructor*/		~main_win			()
	{
		//
	}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	main_win w;
	w.resize(640, 480);
	w.show();

	return a.exec();
}
