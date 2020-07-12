#include "rentcalendar.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RentCalendar w;
	w.show();
	return a.exec();
}
