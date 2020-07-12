#ifndef RESERVATION_H
#define RESERVATION_H

#include <QObject>
#include <QDataStream>
#include <QDate>
#include <QTime>
#include "item.h"
#include "status.h"

class Reservation
{
public:
	Reservation();
	~Reservation();
	Reservation
		(
		QDate startDate, QTime startTime, QDate endDate, QTime endTime, int numberOfDays,
		Status status, QString note,
		QString fio, QString passport, QString drivingLicense, QString address, QString telephoneNumber,
		double deposit, double totalSum, double alreadyPaid, int dayPrice,
		QString addInfo, Item item, unsigned int id
		);

	Reservation & operator=(const Reservation &other);

	/* Перегрузка операции выгрузки в поток для класса Reservation */
	friend QDataStream & operator<<(QDataStream &stream, const Reservation &other);

	/* Перегрузка операции выгрузки из потока для класса Reservation */
	friend QDataStream & operator>>(QDataStream &stream, Reservation &other);

				/* Сведения о дате резервации */
	QDate startDate;			// Дата начала резервации
	QTime startTime;			// Время начала резервации
	QDate endDate;				// Дата конца резервации
	QTime endTime;				// Время конца резервации
	int   numberOfDays;			// Количество дней резервации

	Status  status;				// Состояние на момент резервации
	QString note;				// Пометка

			/* Сведения о человеке, оформляющем резервацию */
	QString fio;				// ФИО 
	QString passport;			// Паспорт 
	QString drivingLicense;		// Вод. удостоверение 
	QString address;			// Адрес 
	QString telephoneNumber;	// Телефонный номер
	
					/* Денежные сведения */
	double deposit;				// Депозит
	double totalSum;			// Общая сумма
	double alreadyPaid;			// Оплачено
	int    dayPrice;			// Цена за сутки

	QString addInfo;			// Дополнительная информация
	Item    item;				// Резервируемое наименование (товар)
	unsigned int id;			// Уникальный идентификатор записи
};

#endif // RESERVATION_H
