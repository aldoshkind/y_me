#include <QWidget>
#include <QTimer>
#include <SFML/Graphics.hpp>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
	Q_OBJECT

public:
	/*constructor*/				QSFMLCanvas			(QWidget *Parent = NULL);
	virtual /*destructor*/		~QSFMLCanvas		();

private:
	virtual void				OnInit				() = 0;
	virtual void				OnUpdate			() = 0;

	virtual QPaintEngine		*paintEngine		() const;
	virtual void				showEvent			(QShowEvent *);
	virtual void				paintEvent			(QPaintEvent *);

	QTimer						myTimer;
	bool						myInitialized;
};

