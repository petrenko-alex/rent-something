#include "rentadd.h"

RentAdd::RentAdd(QVector<Status> *sdir, QVector<Item> *idir, QVector<Reservation> *rdir)
{
	ui.setupUi(this);
	QWidget::setWindowModality(Qt::ApplicationModal);
	/* Принимаем текущую базу состояний и наименований */
	this->sdir = sdir;
	this->idir = idir;
	this->rdir = rdir;
	

	connections();
	currentItemIndex = 0;
	currentResId = -1;
}

RentAdd::~RentAdd()
{

}

void RentAdd::addNewReservation(QDate &date, int itemIndex)
{
	clearForm();
	/* Инициализируем поля с датами и временем */
	ui.startDate->setDate(date);
	ui.startTime->setTime(QTime::currentTime());
	ui.endDate->setDate(date);
	ui.endTime->setTime(QTime::currentTime());

	/* Инициализируем выпадающий список состояний */
	initializeStatusBox();

	/* Инициализируем выпадающий список суточных цен */
	initializePricesBox(itemIndex);

	/* Инициализируем поля */
	ui.totalSum->setValue(((*idir)[itemIndex].getPrices())[0]);
	ui.deposit->setValue(0.0);
	ui.alreadyPaid->setValue(0.0);
	ui.numberOfDays->setValue(1);
	
	/* Инициализируем переменные класса */
	this->currentItemIndex = itemIndex;
	this->currentMode = ADD;
	this->currentResId = -1;
	/* Инициализируем имя окна */
	QString title = "Наименование " + (*idir)[itemIndex].getName();
	this->setWindowTitle(title);
}


void RentAdd::editExistingReservation(Reservation &res, int itemIndex,unsigned int curResId)
{
	clearForm();
	/* Инициализируем выпадающий список состояний */
	initializeStatusBox();
	/* Инициализируем выпадающий список суточных цен */
	initializePricesBox(itemIndex);
	/* Инициализируем поля по данным записи res */
	ui.startDate->setDate(res.startDate);
	ui.startTime->setTime(res.startTime);
	ui.endDate->setDate(res.endDate);
	ui.endTime->setTime(res.endTime);
	ui.numberOfDays->setValue(res.numberOfDays);

	ui.status->setCurrentIndex(findStatusIndex(res.status.getName()));
	ui.note->setText(res.note);

	ui.fio->setText(res.fio);
	ui.passport->setText(res.passport);
	ui.drivingLicense->setText(res.drivingLicense);
	ui.address->setText(res.address);
	ui.telephoneNumber->setText(res.telephoneNumber);

	ui.deposit->setValue(res.deposit);
	ui.totalSum->setValue(res.totalSum);
	ui.alreadyPaid->setValue(res.alreadyPaid);
	ui.addInfo->setText(res.addInfo);

	int indexOfDayPrice = getIndexOfDayPrice(res.dayPrice, res.item);
	ui.dayPrice->setCurrentIndex(indexOfDayPrice);

	/* Инициализируем переменные класса */
	this->currentMode = EDIT;
	this->currentItemIndex = itemIndex;
	this->currentResId = curResId;

	/* Инициализируем имя окна */
	QString title = "Наименование " + (*idir)[itemIndex].getName();
	this->setWindowTitle(title);
}

void RentAdd::okButtonClicked()
{
	/* Проверка полей ввода */
	if (checkData())
	{
		/* Считывание данных из полей ввода */
		Reservation record;
		record = readData();
		record.id = -1;

		/* Проверка дат */
		if (!isItemBusy(record))
		{
			/* Испускание сигнала с новой записью */
			if (currentMode == ADD)
			{
				if (record.status == (*sdir)[0])
				/* Нельзя добавить запись со статусом по умолчанию */
				{
					QMessageBox::warning(this, "Внимание!", "Вы пытаетесь добавить запись с состоянием \"Свободен\".\nЭто состояние предназначено для освобождения наименования от резервации. Пожалуйста, задайте другое состояние.");
					return;
				}
				emit addReservation(record);
			}
			else if (currentMode == EDIT)
			{
				if (record.status.getName() == "Свободен")
				{
					/* Создаем предупреждающее диалоговое окно */
					QMessageBox warning(QMessageBox::Question, "Внимание!", "При изменении состояния текущей записи на \"Свободен\", запись будет удалена.\n\t\t\tПродолжить выполнение?");
					warning.addButton("Да", QMessageBox::YesRole);
					warning.addButton("Нет", QMessageBox::NoRole);
					/* Получаем ответ пользователя */
					int userChoice = warning.exec();
					/* Если пользователь отказался продолжать выполнение */
					if (userChoice == 1)
					{
						return;
					}
				}
				record.id = currentResId;
				emit editReservation(record);
			}

			this->close();
		}
		
	}
} 

void RentAdd::cancelButtonClicked()
{
	this->close();
}

void RentAdd::updateFields()
{
	/* Обновить поле, отображающее количество дней бронирования по разности между датой конца и начала */
	QDate tmpStartDate = ui.startDate->date();
	QDate tmpEndDate   = ui.endDate->date();
	int numberOfDays   = tmpStartDate.daysTo(tmpEndDate) + 1;
	ui.numberOfDays->setValue(numberOfDays);
	/* Обновить поле "За сутки" по количеству дней бронирования */
	setDayPrice(numberOfDays);
	/* Обновить поле "Сумма" */
	int dayPrice = getDayPrice(numberOfDays);
	ui.totalSum->setValue(dayPrice*numberOfDays);

}

void RentAdd::clearForm()
{
	ui.startDate->clear();
	ui.numberOfDays->clear();
	ui.startTime->clear();
	ui.endDate->clear();
	ui.endTime->clear();
	ui.note->clear();
	ui.fio->clear();
	ui.passport->clear();
	ui.drivingLicense->clear();
	ui.address->clear();
	ui.telephoneNumber->clear();
	ui.deposit->clear();
	ui.addInfo->clear();
	ui.totalSum->clear();
	ui.alreadyPaid->clear();
	ui.status->clear();
	ui.dayPrice->clear();
}

void RentAdd::connections()
{
	connect(ui.rentAddOkB, SIGNAL(clicked(bool)), SLOT(okButtonClicked()));
	connect(ui.rentAddCancelB, SIGNAL(clicked(bool)), SLOT(cancelButtonClicked()));
	connect(ui.startDate, SIGNAL(dateChanged(const QDate &)), SLOT(updateFields()));
	connect(ui.endDate, SIGNAL(dateChanged(const QDate &)), SLOT(updateFields()));
}

bool RentAdd::checkData()
{
	bool isCorrect = true;

	/* Проверка корректности дат */
	if (ui.endDate->date() < ui.startDate->date())
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Дата начала проката не может быть позднее даты окончания.\n\tПроверьте корректность полей ввода дат.");
		isCorrect = false;
	}
	else if (ui.endDate->date() == ui.startDate->date() && ui.endTime->time() <= ui.startTime->time())
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Время приемки не может быть меньше времени выдачи.\n\tПроверьте корректность полей ввода времени.");
		isCorrect = false;
	}
	/* Проверка, заполнены ли обязательные поля */
	else if (ui.fio->text() == "")
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Заполните поле \"ФИО\".\t\t");
		isCorrect = false;
	}
	else if (ui.passport->text() == "")
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Заполните поле \"Паспорт\".\t\t");
		isCorrect = false;
	}
	else if (ui.drivingLicense->text() == "")
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Заполните поле \"Вод.удостоверение\".\t\t");
		isCorrect = false;
	}
	else if (ui.address->text() == "")
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Заполните поле \"Адрес\".\t\t");
		isCorrect = false;
	}
	else if (ui.telephoneNumber->text() == "")
	{
		QMessageBox::warning(this, "Ошибка добавления записи", "Заполните поле \"Телефон\".\t\t");
		isCorrect = false;
	}
	return isCorrect;

}

Reservation RentAdd::readData()
{
	Reservation record;

	record.startDate = ui.startDate->date();
	record.startTime = ui.startTime->time();
	record.endDate = ui.endDate->date();
	record.endTime = ui.endTime->time();
	record.numberOfDays = ui.numberOfDays->value();

	record.status = (*sdir)[findStatusIndex(ui.status->currentText())];
	record.note = ui.note->text();

	record.fio = ui.fio->text();
	record.passport = ui.passport->text();
	record.drivingLicense = ui.drivingLicense->text();
	record.address = ui.address->text();
	record.telephoneNumber = ui.telephoneNumber->text();

	record.deposit = ui.deposit->value();
	record.addInfo = ui.addInfo->text();

	record.totalSum = ui.totalSum->value();
	record.alreadyPaid = ui.alreadyPaid->value();

	record.item = (*idir)[currentItemIndex];
	record.dayPrice = getDayPrice(record.numberOfDays);

	return record;
}

int RentAdd::findStatusIndex(const QString &statusName)
{
	int i = 0;
	for (auto it : *sdir)
	{
		if (it.getName() == statusName)
			return i;
		++i;
	}
	return -1;
}

int RentAdd::getDayPrice(int numberOfDays)
{
	if (numberOfDays > 0 && numberOfDays < 3)
		return (*idir)[currentItemIndex].getPrices()[0];
	else if (numberOfDays > 2 && numberOfDays < 8)
		return (*idir)[currentItemIndex].getPrices()[1];
	else if (numberOfDays > 7 && numberOfDays < 15)
		return (*idir)[currentItemIndex].getPrices()[2];
	else if (numberOfDays > 14 && numberOfDays < 31)
		return (*idir)[currentItemIndex].getPrices()[3];
	else if (numberOfDays > 30)
		return (*idir)[currentItemIndex].getPrices()[4];
}

void RentAdd::setDayPrice(int numberOfDays)
{
	if (numberOfDays > 0 && numberOfDays < 3)
		ui.dayPrice->setCurrentIndex(0);
	else if (numberOfDays > 2 && numberOfDays < 8)
		ui.dayPrice->setCurrentIndex(1);
	else if (numberOfDays > 7 && numberOfDays < 15)
		ui.dayPrice->setCurrentIndex(2);
	else if (numberOfDays > 14 && numberOfDays < 31)
		ui.dayPrice->setCurrentIndex(3);
	else if (numberOfDays > 30)
		ui.dayPrice->setCurrentIndex(4);
}

int RentAdd::getIndexOfDayPrice(int dayPrice, Item &item)
{
	for (int i = 0; i < 5; ++i)
	{
		if (item.getPrices()[i] == dayPrice)
			return i;
	}
	return -1;
}

void RentAdd::initializeStatusBox()
{
	ui.status->clear();
	int sdirSize = sdir->size();
	for (int i = 0; i < sdirSize; ++i)
	{
		ui.status->addItem((*sdir)[i].getName(), i);
	}
}

void RentAdd::initializePricesBox(int itemIndex)
{
	ui.dayPrice->clear();
	QStringList prices;
	prices << "1-2 дня\t" << "3-7 дней\t" << "8-14 дней\t" << "15-30 дней\t" << "30 и более\t";
	for (int i = 0; i < NUMBEROFPRICES; ++i)
	{
		QString tmpString = prices[i] + QString::number(((*idir)[itemIndex].getPrices())[i], 'f', 2);
		ui.dayPrice->addItem(tmpString);
	}
}

bool RentAdd::isItemBusy(Reservation &res)
{
	/* Проходим по вектору резерваций */
	for (auto it : *rdir)
	{
		/* Если наименование в добавляемой записи совпадает с наименованием в записи резервации */
		if (it.item == res.item)
		{
			/* Пропуск записи если происходит сравнение с текущей при редактировании */
			if (currentMode == EDIT && it.id == currentResId)
				continue;

			/* Условия пересечения дат */
			bool condition1 = res.startDate >= it.startDate && res.startDate <= it.endDate;
			bool condition2 = res.startDate < it.startDate && res.endDate >= it.startDate && res.endDate <= it.endDate;
			bool condition3 = res.startDate < it.startDate && res.endDate > it.endDate;

			/* Если происходит пересечение дат */
			if (condition1 || condition2 || condition3)
			{
				/* Выводим сообщение */
				QString warningText = "На период с " + res.startDate.toString("dd/MM/yy") + " по " + res.endDate.toString("dd/MM/yy") + " " + res.item.getName() + " с номером " + res.item.getNumber() + " не доступен для резервации!";
				QMessageBox::warning(this, "Проверка дат", warningText);
				return true;
			}
		}
	}
	return false;
}
