/* apbar.h */

#ifndef APBAR_H
#define APBAR_H

class apbar : public QWidget
{
	typedef struct
	{
		Window w;
		QString name;
		int width;
		int number;
	} WINDOW;
	QList <WINDOW*> ap;  // installed clients
	
protected:
	virtual void paletteChange(const QPalette &);

public:
	apbar(QWidget *parent=0);
	void place_clients(void);          // place clients on toolbar
	void release_all(void);            // reparent back to root window
	bool add(Window, int, QString &);  // add client
	bool client_exists(Window);        // client on list
	bool remove(Window);               // remove client
	void remove(void);                 // remove old clients after configure
};
#endif
