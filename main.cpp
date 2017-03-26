#include <queue>

#include <unistd.h>

#include <QApplication>

#include "main_widget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	main_widget w;
	w.resize(640, 480);
	w.show();

	return a.exec();
}
