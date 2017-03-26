#pragma once

#include <deque>

#include <unistd.h>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QFileInfo>

#include <curl/curl.h>

#include <SFML/Graphics.hpp>

#include "tiler.h"

class texture_cache : public QThread
{
	Q_OBJECT

	QMutex					mutex;
	QWaitCondition			condvar;
	bool					running;
	tiler					*t;

	std::map<int, std::map<int, std::map<int, sf::Texture> > > texs;

	struct tile_coord
	{
		int col;
		int row;
		int zoom;

		/*constructor*/			tile_coord			(int c, int r, int z)
		{
			col = c;
			row = r;
			zoom = z;
		}
	};

	QString					get_url					(tile_coord c);

	bool					file_exists				(QString path);

	typedef std::deque<tile_coord>		queue_t;
	queue_t								queue;

	int						last_zoom;

	void					enqueue					(int col, int row, int zoom);
	void					run						();

public:
	/*constructor*/			texture_cache			();
	/*destructor*/			~texture_cache			();

	void					set_tiler				(tiler *t);
	QString					download_img			(tiler *t, int col, int row, int zoom);
	sf::Texture				*get_texture			(int col, int row, int zoom);
	void					loader					();
};
