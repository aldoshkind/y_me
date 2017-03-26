#include "texture_cache.h"

/*constructor*/ texture_cache::texture_cache()
{
	last_zoom = 0;
	running = true;
	t = NULL;
	start();
}

/*destructor*/ texture_cache::~texture_cache()
{
	running = false;
	condvar.wakeOne();
	wait();
}

void texture_cache::enqueue(int col, int row, int zoom)
{
	const QString &path = get_url(tile_coord(col, row, zoom));
	if(file_exists(path))
	{
		texs[col][row][zoom].loadFromFile(path.toStdString());
	}

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

void texture_cache::run()
{
	loader();
}

void texture_cache::set_tiler(tiler *t)
{
	this->t = t;
}

QString texture_cache::download_img(tiler *t, int col, int row, int zoom)
{
	QString path = get_url(tile_coord(col, row, zoom));
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

sf::Texture *texture_cache::get_texture(int col, int row, int zoom)
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

void texture_cache::loader()
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

bool texture_cache::file_exists(QString path)
{
	QFileInfo check_file(path);

	return check_file.exists() && check_file.isFile();
}

QString texture_cache::get_url(tile_coord c)
{
	return QString("/home/dmitry/cache/%1_%2_%3.jpg").arg(c.col).arg(c.row).arg(c.zoom);
}
