#include "item.h"

Item::Item()
{
	this->name		= "";
	this->number	= "";
	this->prices;
}

Item::Item(QString name, QString number, QVector<float> &prices)
{
	this->name   = name;
	this->number = number;
	this->prices = prices;
}

Item & Item::operator=(const Item &other)
{
	this->name	 = other.name;
	this->number = other.number;
	this->prices = other.prices;
	return *this;
}

QString Item::getName()
{
	return this->name;
}

void Item::setName(QString &name)
{
	this->name = name;
}

QString Item::getNumber()
{
	return this->number;
}

void Item::setNumber(QString &number)
{
	this->number = number;
}

QVector<float>& Item::getPrices()
{
	return this->prices;
}

void Item::setPrices(QVector<float> &prices)
{
	this->prices = prices;
}

QDataStream & operator>>(QDataStream &stream, Item &other)
{
	stream >> other.name >> other.number >> other.prices;
	return stream;
}

QDataStream & operator<<(QDataStream &stream, const Item &other)
{
	stream << other.name << other.number << other.prices;
	return stream;
}

bool Item::operator==(const Item &other)
{
	return this->name   == other.name
		&& this->number == other.number
		&& this->prices == other.prices;
}

Item::~Item()
{
}
