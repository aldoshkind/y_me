#pragma once

#include <deque>
#include <set>

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

		bool					operator <			(const tile_coord &op) const
		{
			if(col < op.col)
			{
				return true;
			}
			if(col == op.col)
			{
				if(row < op.row)
				{
					return true;
				}
				if(row == op.row)
				{
					return zoom < op.zoom;
				}
			}
			return false;
		}
	};

	typedef std::deque<tile_coord>		queue_t;
	queue_t								queue;

	typedef std::set<tile_coord>		current_queue_items_t;
	current_queue_items_t				current_queue_items;

	int						last_zoom;

	QString					get_url					(tile_coord c);

	bool					file_exists				(QString path);

	void					enqueue					(int col, int row, int zoom);
	void					run						();

public:
	/*constructor*/			texture_cache			();
	/*destructor*/			~texture_cache			();

	void					set_tiler				(tiler *t);
	QString					download_img			(int col, int row, int zoom);
	sf::Texture				*get_texture			(int col, int row, int zoom);
	void					loader					();
};
