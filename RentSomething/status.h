#ifndef STATUS_H
#define STATUS_H

#include <QObject>
#include <QDataStream>
#include <QColor>

class Status
{
public:
	Status();
	~Status();

	Status(QString name, QColor &color);
	bool operator==(const Status &other);
	Status & operator=(const Status &other);


	QString getName();
	void	setName(QString &name);

	QColor  getColor();
	void    setColor(QColor &color);

	/* Перегрузка операции выгрузки в поток для класса Status */
	friend QDataStream & operator<<(QDataStream &stream, const Status &other);

	/* Перегрузка операции выгрузки из потока для класса Status */
	friend QDataStream & operator>>(QDataStream &stream, Status &other);

private:
	QString name;
	QColor  color;
};

#endif // STATUS_H
