#include "reservation.h"

Reservation::Reservation()
{
	startDate		= QDate::currentDate();
	startTime		= QTime::currentTime();
	endDate			= QDate::currentDate();
	endTime			= QTime::currentTime();
	numberOfDays	= 1;

	status;
	note			= "";

	fio				= "";
	passport		= "";
	drivingLicense	= "";
	address			= "";
	telephoneNumber = "";

	deposit			= 0;
	totalSum		= 0;
	alreadyPaid		= 0;
	dayPrice		= 0;

	addInfo			= "";
	Item;
	id				= 0;
}

Reservation::Reservation(QDate startDate, QTime startTime, QDate endDate, QTime endTime, int numberOfDays, Status status, QString note, QString fio, QString passport, QString drivingLicense, QString address, QString telephoneNumber, double deposit, double totalSum, double alreadyPaid, int dayPrice, QString addInfo, Item item,unsigned int id)
{
	this->startDate		  = startDate;
	this->startTime		  = startTime;
	this->endDate		  = endDate;
	this->endTime		  = endTime;
	this->numberOfDays    = numberOfDays;

	this->status		  = status;
	this->note			  = note;

	this->fio			  = fio;
	this->passport		  = passport;
	this->drivingLicense  = drivingLicense;
	this->address		  = address;
	this->telephoneNumber = telephoneNumber;

	this->deposit		  = deposit;
	this->totalSum		  = totalSum;
	this->alreadyPaid	  = alreadyPaid;
	this->dayPrice		  = dayPrice;

	this->addInfo		  = addInfo;
	this->item			  = item;
	this->id			  = id;
}

QDataStream & operator>>(QDataStream &stream, Reservation &other)
{
	stream	>> other.startDate
			>> other.startTime
			>> other.endDate
			>> other.endTime
			>> other.numberOfDays
			>> other.status
			>> other.note
			>> other.fio
			>> other.passport
			>> other.drivingLicense
			>> other.address
			>> other.telephoneNumber
			>> other.deposit
			>> other.totalSum
			>> other.alreadyPaid
			>> other.dayPrice
			>> other.addInfo
			>> other.item
			>> other.id;

	return stream;
}

QDataStream & operator<<(QDataStream &stream, const Reservation &other)
{
	stream	<< other.startDate
			<< other.startTime
			<< other.endDate
			<< other.endTime
			<< other.numberOfDays
			<< other.status
			<< other.note
			<< other.fio
			<< other.passport
			<< other.drivingLicense
			<< other.address
			<< other.telephoneNumber
			<< other.deposit
			<< other.totalSum
			<< other.alreadyPaid
			<< other.dayPrice
			<< other.addInfo
			<< other.item
			<< other.id;

	return stream;
}

Reservation & Reservation::operator=(const Reservation &other)
{
	this->startDate			= other.startDate;
	this->startTime			= other.startTime;
	this->endDate			= other.endDate;
	this->endTime			= other.endTime;
	this->numberOfDays		= other.numberOfDays;

	this->status			= other.status;
	this->note				= other.note;

	this->fio				= other.fio;
	this->passport			= other.passport;
	this->drivingLicense	= other.drivingLicense;
	this->address			= other.address;
	this->telephoneNumber	= other.telephoneNumber;

	this->deposit			= other.deposit;
	this->totalSum			= other.totalSum;
	this->alreadyPaid		= other.alreadyPaid;
	this->dayPrice			= other.dayPrice;

	this->addInfo			= other.addInfo;
	this->item				= other.item;
	this->id				= other.id;

	return *this;
}

Reservation::~Reservation()
{

}
