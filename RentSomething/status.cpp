#include "status.h"

Status::Status()
{
	name = "";
	color.setNamedColor ("white");
}

Status::Status(QString name, QColor &color)
{
	this->name  = name;
	this->color = color;
}

Status & Status::operator=(const Status &other)
{
	this->name = other.name;
	this->color = other.color;
	return *this;
}

QString Status::getName()
{
	return this->name;
}

void Status::setName(QString &name)
{
	this->name = name;
}

QColor Status::getColor()
{
	return this->color;
}

void Status::setColor(QColor &color)
{
	this->color = color;
}

QDataStream & operator>>(QDataStream &stream, Status &other)
{
	stream >> other.name >> other.color;
	return stream;
}

QDataStream & operator<<(QDataStream &stream, const Status &other)
{
	stream << other.name << other.color;
	return stream;
}

bool Status::operator==(const Status &other)
{
	return this->name  == other.name
		&& this->color == other.color;
}

Status::~Status()
{
}
