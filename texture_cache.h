#ifndef TEXTURE_CACHE_H
#define TEXTURE_CACHE_H

#include <deque>

#include <unistd.h>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

#include <curl/curl.h>

#include <SFML/Graphics.hpp>

#include "tiler.h"

class texture_cache : public QThread
{
	Q_OBJECT

	QMutex					mutex;
	QWaitCondition			condvar;
	bool					running;

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

	typedef std::deque<tile_coord>		queue_t;
	queue_t								queue;

	int						last_zoom;

	void					enqueue					(int col, int row, int zoom)
	{
		mutex.lock();
		if(last_zoom != zoom)
		{
			queue_t q;
			std::swap(q, queue);
		}
		last_zoom = zoom;
		queue.push_front(tile_coord(col, row, zoom));
		mutex.unlock();
		condvar.wakeOne();
	}

	tiler					*t;

	void					run						()
	{
		loader();
	}

public:
	/*constructor*/			texture_cache			()
	{
		last_zoom = 0;
		running = true;
		t = NULL;
		start();
	}

	/*destructor*/			~texture_cache			()
	{
		running = false;
		//qDebug() << "~before condvar";
		condvar.wakeOne();
		wait();
	}

	void					set_tiler				(tiler *t)
	{
		this->t = t;
	}

	std::map<int, std::map<int, std::map<int, sf::Texture> > > texs;

	QString download_img(tiler *t, int col, int row, int zoom)
	{
		QString path = QString("/home/dmitry/cache/%1_%2_%3.jpg").arg(col).arg(row).arg(zoom);
		qDebug() << path;

		if(access(path.toUtf8().data(), F_OK) == 0)
		{
			return path;
		}

		QString url = t->get_url(col, row, zoom);

		FILE *out_file = fopen(path.toUtf8().data(), "wb");

		if(out_file == NULL)
		{
			return "";
		}

		CURL *curl = NULL;
		CURLcode res;

		curl = curl_easy_init();
		if(curl == NULL)
		{
			return "";
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_file);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36");
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			return "";
		}
		curl_easy_cleanup(curl);

		fclose(out_file);

		return path;
	}

	sf::Texture *get_texture(int col, int row, int zoom)
	{
		if(texs.find(col) == texs.end())
		{
			enqueue(col, row, zoom);
		}
		else if(texs[col].find(row) == texs[col].end())
		{
			enqueue(col, row, zoom);
		}
		else if(texs[col][row].find(zoom) == texs[col][row].end())
		{
			enqueue(col, row, zoom);
		}
		else
		{
			return &(texs[col][row][zoom]);
		}

		return NULL;
	}

	void					loader					()
	{
		for( ; running == true ; )
		{
			mutex.lock();
			condvar.wait(&mutex);

			mutex.unlock();

			for( ; running == true ; )
			{
				mutex.lock();
				tile_coord tc = queue.front();
				queue.pop_front();
				mutex.unlock();

				QString path = download_img(t, tc.col, tc.row, tc.zoom);
				texs[tc.col][tc.row][tc.zoom].loadFromFile(path.toStdString());

				mutex.lock();
				if(queue.size() < 1)
				{
					mutex.unlock();
					break;
				}
				mutex.unlock();
			}
		}
	}
};

#endif // TEXTURE_CACHE_H
