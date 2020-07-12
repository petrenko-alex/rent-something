#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QVector>
#include <QDataStream>
#define  NUMBEROFPRICES 5

class Item
{
public:
	Item();
	~Item();

	Item(QString name, QString number, QVector<float> &prices);
	bool operator==(const Item &other);
	Item & operator=(const Item &other);

	QString getName();
	void	setName(QString &name);

	QString getNumber();
	void	setNumber(QString &number);
	
	QVector<float>& getPrices();
	void		  setPrices(QVector<float> &prices);

	/* Перегрузка операции выгрузки в поток для класса Item */
	friend QDataStream & operator<<(QDataStream &stream, const Item &other);

	/* Перегрузка операции выгрузки из потока для класса Item */
	friend QDataStream & operator>>(QDataStream &stream, Item &other);

private:
	QString			name;
	QString			number;
	QVector<float>	prices;
};

#endif // ITEM_H
