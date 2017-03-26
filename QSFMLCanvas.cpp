#include "QSFMLCanvas.h"

/*constructor*/ QSFMLCanvas::QSFMLCanvas(QWidget* Parent) : QWidget(Parent), myInitialized (false)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);

	setFocusPolicy(Qt::StrongFocus);

	int FrameTime = 20;
	myTimer.setInterval(FrameTime);
}

/*destructor*/ QSFMLCanvas::~QSFMLCanvas()
{
	//
}

void QSFMLCanvas::showEvent(QShowEvent*)
{
	if(!myInitialized)
	{
		#ifdef Q_WS_X11
			XFlush(QX11Info::display());
		#endif

		sf::RenderWindow::create((sf::WindowHandle)winId());

		OnInit();

		connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
		myTimer.start();

		myInitialized = true;
	}
}

QPaintEngine* QSFMLCanvas::paintEngine() const
{
	return 0;
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
	// Let the derived class do its specific stuff
	OnUpdate();

	// Display on screen
	display();
}
