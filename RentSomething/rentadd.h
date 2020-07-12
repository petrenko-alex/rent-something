#ifndef RENTADD_H
#define RENTADD_H

#include <QWidget>
#include "ui_rentadd.h"
#include "statusadd.h"
#include "reservation.h"

class RentAdd : public QWidget
{
	Q_OBJECT

public:
	RentAdd(QVector<Status> *sdir,QVector<Item> *idir,QVector<Reservation> *rdir);
	~RentAdd();

	void addNewReservation(QDate &date, int itemIndex);
	void editExistingReservation(Reservation &res, int itemIndex, unsigned int curResId);
	

private slots:
	void okButtonClicked();
	void cancelButtonClicked();
	void updateFields();


signals:
	void addReservation(Reservation &newRes);
	void editReservation(Reservation &newRes);


private:
	void clearForm();
	void connections();
	bool checkData();
	Reservation readData();
	int findStatusIndex(const QString &statusName);
	int getDayPrice(int numberOfDays);
	void setDayPrice(int numberOfDays);
	int getIndexOfDayPrice(int dayPrice, Item &item);
	void initializeStatusBox();
	void initializePricesBox(int itemIndex);
	bool isItemBusy(Reservation &res);
	

	Ui::RentAdd ui;
	QVector<Status> *sdir;
	QVector<Item>   *idir;
	QVector<Reservation> *rdir;

	int currentItemIndex;
	unsigned int currentResId;
	mode currentMode;
};

#endif // RENTADD_H
