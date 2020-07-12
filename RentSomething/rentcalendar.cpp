#include "rentcalendar.h"

RentCalendar::RentCalendar(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	/* Устанавливаем имя файла */
	defaultFileName = "./Data/CurrentData.rsd";
	userFileName = "";

	/* Создаем формы */
	itemDir = new ItemDir(&itemDirectory, &resDirectory);
	statusDir = new StatusDir(&statusDirectory, &resDirectory);
	rentAdd = new RentAdd(&statusDirectory, &itemDirectory, &resDirectory);

	/* Шрифт для календаря */
	QFont newFont = ui.calendar->font();
	newFont.setPixelSize(10);
	ui.calendar->setFont(newFont);

	/* Количество отображаемых колонок */
	ui.calendar->setColumnCount(MAXCOLUMN);

	/* Первоначальная инициализация переменных */
	maxId = 0;
	blockDateChangedSignal = false;
	curCalendarStart = QDate::currentDate().addDays(-2);
	ui.startDate->setDate(curCalendarStart);

	/* Загружаем данные */
	if (loadData())
	{
		/* Инициализируем справочник состояний */
		statusDir->intitializeForm();
		/* Инициализируем справочник наименований */
		itemDir->intitializeForm();
	}
	else
	{
		QMessageBox::information(this, "Внимание!", "Программа была загружена с исходными данными.");
		statusDir->addDefaultStatus();
	}

	/* Инициализируем календарь */
	initializeCalendar();

	/* Соединение сигналов и слотов */
	connections();
}

RentCalendar::~RentCalendar()
{

}

void RentCalendar::closeEvent(QCloseEvent *event)
{
	saveData();
}

void RentCalendar::connections()
{
	connect(ui.itemDirB, SIGNAL(clicked(bool)), SLOT(openItemDirectory()));
	connect(ui.okB, SIGNAL(clicked(bool)), SLOT(closeProgram()));
	connect(ui.statusDirB, SIGNAL(clicked(bool)), SLOT(openStatusDirectory()));
	connect(ui.calendar, SIGNAL(cellDoubleClicked(int,int)), SLOT(openResAdd(int,int)));
	connect(rentAdd, SIGNAL(addReservation(Reservation&)), this, SLOT(addNewRes(Reservation&)));
	connect(rentAdd, SIGNAL(editReservation(Reservation&)), this, SLOT(editExistingRes(Reservation&)));
	connect(ui.nextDayB, SIGNAL(clicked(bool)), SLOT(nextDay()));
	connect(ui.prevDayB, SIGNAL(clicked(bool)), SLOT(prevDay()));
	connect(ui.nextMonthB, SIGNAL(clicked(bool)), SLOT(nextMonth()));
	connect(ui.prevMonthB, SIGNAL(clicked(bool)), SLOT(prevMonth()));
	connect(ui.startDate, SIGNAL(dateChanged(const QDate &)), SLOT(dateChanged()));
	connect(ui.calendar, SIGNAL(cellClicked(int,int)),SLOT(showBriefInfo (int,int)));
	connect(itemDir, SIGNAL(itemDirClosed(bool)), this, SLOT(refreshCalendar(bool)));

}

void RentCalendar::initializeCalendar()
{
	/* Инициализируем заголовки */
	formMonthCalendarColumns(curCalendarStart);
	/* Инициализируем ряды */
	buildCalendarRows();
	/* Инициализируем резервации в текущем календаре */
	addResItems();
}

void RentCalendar::addCalendarRow(Item &newRow)
{
	QTableWidgetItem *item;
	int rowCount = ui.calendar->rowCount();
	/* Вставляем ряд в таблицу */
	ui.calendar->insertRow(rowCount);
	/* Создаем 1-ю ячейку */
	item = new QTableWidgetItem(newRow.getName ());
	ui.calendar->setItem(rowCount, 0, item);
	/* Создаем 2-ю ячейку */
	item = new QTableWidgetItem(newRow.getNumber());
	ui.calendar->setItem(rowCount, 1, item);
	/* Создаем 3-ю ячейку */
	item = new QTableWidgetItem(QString::number(newRow.getPrices()[0]));
	ui.calendar->setItem(rowCount, 2, item);
	/* Настройки таблицы */
	ui.calendar->setRowHeight(rowCount, 25);
	ui.calendar->setRowCount(++rowCount);
}

void RentCalendar::addCalendarItem(int row, int column, Reservation &newRes)
{
	/* Формируем данные для ячейки */
	struct calendarItem itemData;
	itemData.resId = newRes.id;
	itemData.status = newRes.status;
	QVariant QVariantitemData;
	QVariantitemData.setValue(itemData);
	/* Устанавливаем цвет и данные ячейки */
	QTableWidgetItem *item;
	item = new QTableWidgetItem();
	QBrush tmpBrush(QBrush(newRes.status.getColor()));
	item->setBackground(tmpBrush);
	item->setData(Qt::UserRole, QVariantitemData);
	ui.calendar->setItem(row, column, item);
}

void RentCalendar::delCalendatItem(int row, int column)
{
	delete ui.calendar->takeItem(row, column);
}

int RentCalendar::findColumnByDate(const QDate &date)
{
	/* Поиск начинается с 3 колонки */
	int i = 3;
	QTableWidgetItem *item;

	do
	{
		/* Получаем дату из ячейки */
		item = ui.calendar->horizontalHeaderItem(i);
		if (item != NULL)
		{
			if (item->data(Qt::UserRole) == date)
				return i;
		}
		else if (item == NULL)
			return -1;

		++i;
	} while (item != NULL);
}

int RentCalendar::findItemIndexByNumber(const QString &number)
{
	int i = 0;
	for (auto it : itemDirectory)
	{
		if (it.getNumber() == number)
			return i;
		++i;
	}
	return -1;
}

int RentCalendar::findResIndexById(const unsigned int id)
{
	int i = 0;
	for (auto it : resDirectory)
	{
		if (it.id == id)
			return i;
		++i;
	}
	return -1;
}

int RentCalendar::findRowNumber(const QString &itemNumber)
{
	int rowCount = ui.calendar->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		QString rowItemNumber = ui.calendar->item(i, 1)->text();
		if (rowItemNumber == itemNumber)
			return i;
	}
	return -1;
}

void RentCalendar::addReservationToCalendar(Reservation &newRes)
{
	int i = 0;
	int curRow = ui.calendar->currentRow();

	if (newRes.startDate >= curCalendarStart && newRes.startDate <= curCalendarEnd)
	{
		QDate tmpDate = newRes.startDate;
		int curColumn = findColumnByDate(newRes.startDate);
		for (; tmpDate <= newRes.endDate && tmpDate <= curCalendarEnd;)
		{
			/* Добавляем ячейку в календарь */
			addCalendarItem(curRow, curColumn, newRes);
			/* Переходим к следующей ячейке */
			tmpDate = tmpDate.addDays(1);
			/* Следующая колонка */
			curColumn += 1;
		}
	}
	else if (newRes.startDate < curCalendarStart && newRes.endDate >= curCalendarStart && newRes.endDate <= curCalendarEnd)
	{
		QDate tmpDate = newRes.endDate;
		int curColumn = findColumnByDate(newRes.endDate);
		for (; tmpDate >= newRes.startDate && tmpDate >= curCalendarStart;)
		{
			/* Добавляем ячейку в календарь */
			addCalendarItem(curRow, curColumn, newRes);
			/* Переходим к следующей ячейке */
			tmpDate = tmpDate.addDays(-1);
			/* Предыдущая колонка колонка */
			curColumn -= 1;
		}
	}
	else if (newRes.startDate < curCalendarStart && newRes.endDate > curCalendarEnd)
	{
		QDate tmpStartDate = newRes.startDate;
		QString tmp = curCalendarStart.toString();

		while (tmpStartDate < curCalendarStart)
		{
			tmpStartDate = tmpStartDate.addDays(1);
		}
		tmp = tmpStartDate.toString();

		for (; tmpStartDate <= curCalendarEnd;)
		{
			/* Текущая колонка */
			int curColumn = 3 + i;
			/* Добавляем ячейку в календарь */
			addCalendarItem(curRow, curColumn, newRes);
			/* Переходим к следующей ячейке */
			tmpStartDate = tmpStartDate.addDays(1);
			++i;
		}
	}
}

void RentCalendar::delReservatrionFromCalendar(Reservation &oldRes)
{
	int i = 0;
	int curRow = ui.calendar->currentRow();

	/* startDate внутри текущего календаря(включая границы);endDate может быть внутри или за пределами текущего календаря*/
	if (oldRes.startDate >= curCalendarStart && oldRes.startDate <= curCalendarEnd)
	{
		QDate tmpDate = oldRes.startDate;
		int curColumn = findColumnByDate(oldRes.startDate);
		for (; tmpDate <= oldRes.endDate && tmpDate <= curCalendarEnd;)
		{
			/* Добавляем ячейку в календарь */
			delCalendatItem(curRow, curColumn);
			/* Переходим к следующей ячейке */
			tmpDate = tmpDate.addDays(1);
			/* Следующая колонка */
			curColumn += 1;
		}
	}
	/* startDate до текущего календаря, а endDate внутри текущего календаря(включая границы) */
	else if (oldRes.startDate < curCalendarStart && oldRes.endDate >= curCalendarStart && oldRes.endDate <= curCalendarEnd)
	{
		QDate tmpDate = oldRes.endDate;
		int curColumn = findColumnByDate(oldRes.endDate);
		for (; tmpDate >= oldRes.startDate && tmpDate >= curCalendarStart;)
		{
			/* Добавляем ячейку в календарь */
			delCalendatItem(curRow, curColumn);
			/* Переходим к следующей ячейке */
			tmpDate = tmpDate.addDays(-1);
			/* Предыдущая колонка колонка */
			curColumn -= 1;
		}
	}
	/* startDate до текущего календаря, а endDate после */
	else if (oldRes.startDate < curCalendarStart && oldRes.endDate > curCalendarEnd)
	{
		QDate tmpStartDate = oldRes.startDate;
		QString tmp = curCalendarStart.toString();

		while (tmpStartDate < curCalendarStart)
		{
			tmpStartDate = tmpStartDate.addDays(1);
		}
		tmp = tmpStartDate.toString();

		for (; tmpStartDate <= curCalendarEnd;)
		{
			/* Текущая колонка */
			int curColumn = 3 + i;
			/* Добавляем ячейку в календарь */
			delCalendatItem(curRow, curColumn);
			/* Переходим к следующей ячейке */
			tmpStartDate = tmpStartDate.addDays(1);
			++i;
		}
	}
}

void RentCalendar::formMonthCalendarColumns(const QDate &startDate)
{
	QTableWidgetItem *item;
	QDate tmpDate = startDate;

	/* Инициализируем календарь на месяц вперед */
	for (int i = 3; i < MAXCOLUMN; ++i)
	{
		/* Формируем строку для колонки */
		QString columnName = tmpDate.toString("d") + "\n" + tmpDate.toString("MMM");

		/* Создаем колонку на основе имени */
		item = new QTableWidgetItem(columnName);
		QVariant date(tmpDate);
		item->setData(Qt::UserRole, date);

		/* Цвет текста ячейки (выходные - красные, будние дни - черный) */
		if (tmpDate.dayOfWeek() == 6 || tmpDate.dayOfWeek() == 7)
			item->setForeground(Qt::red);
		else
			item->setForeground(Qt::black);

		/* Жирный шрифт для сегодняшнего дня */
		if (tmpDate == QDate::currentDate())
		{
			QFont tmpBoldFont = ui.calendar->font();
			tmpBoldFont.setBold(true);
			item->setFont(tmpBoldFont);
		}
		/* Устанавливаем колонку в таблицу */
		ui.calendar->setHorizontalHeaderItem(i, item);
		/* Устанавливаем размер колонки по высоте */
		ui.calendar->setColumnWidth(i, 40);
		/* Переходим к следующему дню */
		tmpDate = tmpDate.addDays(1);
	}
	tmpDate = tmpDate.addDays(-1);
	curCalendarEnd = tmpDate;
}

void RentCalendar::deleteResItems()
{
	int rowCount = ui.calendar->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 3; j < MAXCOLUMN; ++j)
		{
			QTableWidgetItem *tmpItem = ui.calendar->item(i, j);
			if (tmpItem != NULL)
			{
				delCalendatItem(i, j);
			}
		}
	}
}

void RentCalendar::deleteCalendarHeaderItems()
{
	for (int i = 3; i < MAXCOLUMN; ++i)
	{
		delete ui.calendar->takeHorizontalHeaderItem(i);
	}
}

void RentCalendar::addResItems()
{
	if (resDirectory.count() != 0)
	{
		for (auto it : resDirectory)
		{
			/* Определяем ряд по номеру наименования */
			QString itemNumber = it.item.getNumber();
			int rowNumber = findRowNumber(itemNumber);
			/* Выделяем текущий ряд */
			ui.calendar->setCurrentCell(rowNumber, 0);
			/* Добавляем запись в календарь */
			addReservationToCalendar(it);
		}
	}
}

void RentCalendar::rebuildCurCalendar()
{
	/* Очистить текущии ячейки с резервациями */
	deleteResItems();

	/* Очистить текущии заголовки */
	deleteCalendarHeaderItems();

	/* Построить новые заголовки */
	formMonthCalendarColumns(curCalendarStart);

	/* Построить новые ячейки с резервациями */
	addResItems();

	/* Новое значение для поля "Дата начала" */
	blockDateChangedSignal = true;
	ui.startDate->setDate(curCalendarStart);
	blockDateChangedSignal = false;
}

void RentCalendar::deleteCalendarRows()
{
	int rowCount = ui.calendar->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		delCalendatItem(i, 0);
		delCalendatItem(i, 1);
		delCalendatItem(i, 2);
		ui.calendar->removeRow(i);
	}
	ui.calendar->setRowCount(0);
}

void RentCalendar::buildCalendarRows()
{
	int itemDirSize = itemDirectory.size();
	for (int i = 0; i < itemDirSize; ++i)
	{
		addCalendarRow(itemDirectory[i]);
		/* Светло зеленый */
		QColor green(189, 255, 211);
		ui.calendar->item(i, 0)->setBackground(QBrush(green));
		ui.calendar->item(i, 1)->setBackground(QBrush(green));
		ui.calendar->item(i, 2)->setBackground(QBrush(green));
	}
}

void RentCalendar::openItemDirectory()
{	
	itemDir->show();
}

void RentCalendar::openStatusDirectory()
{
	statusDir->show();
}

void RentCalendar::closeProgram()
{
	this->close();
}

void RentCalendar::openResAdd(int row, int column)
{
	if (column > 2)
	{
		/* Получаем индекс текущего наименования */
		int curRow = ui.calendar->currentRow();
		QString indexNumber = ui.calendar->item(curRow, 1)->text();
		int itemIndex = findItemIndexByNumber(indexNumber);

		/* Если на эту дату еще нет резервации */
		if (ui.calendar->currentItem() == NULL)
		{
			/* Определяем дату в ячейке */
			QDate tmpDate = ui.calendar->horizontalHeaderItem(column)->data (Qt::UserRole).toDate ();
			/* Инициализируем форму в режиме добавления новой записи */
			rentAdd->addNewReservation(tmpDate, itemIndex);
			
		}
		/* Если на эту дату уже есть резервация */
		else if (ui.calendar->currentItem() != NULL)
		{
			/* Получаем данные ячейки */
			QVariant variantItemData = ui.calendar->currentItem()->data(Qt::UserRole);
			struct calendarItem  itemData = variantItemData.value<calendarItem>();
			/* Инициализируем форму в режиме редактирования существующей записи */
			int currentResIndex = findResIndexById(itemData.resId);
			rentAdd->editExistingReservation(resDirectory[currentResIndex], itemIndex,itemData.resId);
		}

		rentAdd->show();
	}
}

void RentCalendar::addNewRes(Reservation &newRes)
{
	/* Добавляем новую запись о резервации в вектор */
	newRes.id = maxId++;
	resDirectory << newRes;
	/* Добавляем запись в календарь */
	addReservationToCalendar(newRes);
}

void RentCalendar::editExistingRes(Reservation &existingRes)
{
	/* Индекс записи в векторе */
	int recordIndex = findResIndexById(existingRes.id); 
	/* Удалить из календаря текущую запись */
	delReservatrionFromCalendar(resDirectory[recordIndex]);
	/* Если статус изменен на "Свободен", то удаляем запись из вектора */
	if (existingRes.status.getName() == "Свободен")
	{
		resDirectory.remove(recordIndex);
	}
	/* Если статус не изменен на "Свободен" */
	else
	{
		/* Обновляем запись в векторе */
		resDirectory[recordIndex] = existingRes;
		/* Добавляем в календарь запись с новыми значениями */
		addReservationToCalendar(resDirectory[recordIndex]);
	}
}

void RentCalendar::nextDay()
{
	/* Изменить дату начала текущего календаря */
	curCalendarStart = curCalendarStart.addDays(1);

	/* Перестроить календарь */
	rebuildCurCalendar();
}

void RentCalendar::prevDay()
{
	/* Изменить дату начала текущего календаря */
	curCalendarStart = curCalendarStart.addDays(-1);

	/* Перестроить календарь */
	rebuildCurCalendar();
}

void RentCalendar::nextMonth()
{
	/* Изменить дату начала текущего календаря */
	curCalendarStart = curCalendarStart.addMonths(1);

	/* Перестроить календарь */
	rebuildCurCalendar();
}

void RentCalendar::prevMonth()
{
	/* Изменить дату начала текущего календаря */
	curCalendarStart = curCalendarStart.addMonths(-1);

	/* Перестроить календарь */
	rebuildCurCalendar();
}

void RentCalendar::dateChanged()
{
	if (!blockDateChangedSignal)
	{
		/* Изменить дату начала текущего календаря */
		curCalendarStart = ui.startDate->date();

		/* Перестроить календарь */
		rebuildCurCalendar();
	}
}

void RentCalendar::showBriefInfo(int row, int column)
{
	/* Очищаем поля */
	ui.itemName->clear();
	ui.rentDate->clear();
	ui.status->clear();
	ui.FIO->clear();
	/* Устанавливаем строку "Наименование" */
	ui.itemName->setText(ui.calendar->item(row, 0)->text());
	/* Если выделена колонка с номером больше 2 */
	if (column > 2)
	{
		/* Если в ячейке есть данные о резервации */
		if (ui.calendar->currentItem() != NULL)
		{
			/* Получаем данные ячейки */
			QVariant variantItemData = ui.calendar->currentItem()->data(Qt::UserRole);
			struct calendarItem  itemData = variantItemData.value<calendarItem>();
			int currentResIndex = findResIndexById(itemData.resId);
			/* Получаем текущую запись о резервации */
			Reservation curRes = resDirectory[currentResIndex];
			/* Устанавливаем строку "Дата выдачи" */
			QString tmpStr = curRes.startDate.toString("dd/MM/yyyy") + " в " + curRes.startTime.toString("hh:mm") + " на " + QString::number(curRes.numberOfDays) + " сут.";
			ui.rentDate->setText(tmpStr);
			/* Устанавливаем строку "Статус" */
			ui.status->setText(curRes.status.getName());
			/* Устанавливаем строку "Ф.И.О." */
			ui.FIO->setText(curRes.fio);
		}
		else
		{
			/* Устанавливаем строку "Статус" */
			ui.status->setText("Свободен");
		}
	}
}

void RentCalendar::refreshCalendar(bool itemDirChanged)
{
	if (itemDirChanged)
	{
		/* Очистить текущии ячейки с резервациями */
		deleteResItems();

		/* Очистить ряды */
		deleteCalendarRows();

		/* Построить новые ряды */
		buildCalendarRows();

		/* Построить новые ячейки с резервациями */
		addResItems();
	}
}

void RentCalendar::saveData()
{
	if (userFileName != "")
	{
		QMessageBox warning(QMessageBox::Question, "Внимание", "При загрузке программы вместо исходного файла данных, был открыт пользовательский файл.\nВ какой файл вы хотите сохранить текущие данные?");
		warning.addButton("Пользовательский файл", QMessageBox::ActionRole);
		warning.addButton("Исходный файл", QMessageBox::ActionRole);
		warning.addButton("Оба файла", QMessageBox::ActionRole);
		int userChoice = warning.exec();

		if (userChoice == 0)
		{
			/* Сохранение в пользовательский файл */
			save(userFileName);
		}
		else if (userChoice == 1)
		{
			/* Сохранение в исходный файл */
			save(defaultFileName);
		}
		else if (userChoice == 2)
		{
			/* Сохранение в оба файла */
			save(defaultFileName);
			save(userFileName);
		}
	}
	else
	{
		/* По умолчанию сохраняется в исходный файл */
		save(defaultFileName);
	}
}

bool RentCalendar::loadData()
{
	/* Попытка загрузить данные из файла по умолчанию */
	bool isLoaded = load(defaultFileName);
	
	if (!isLoaded)
	/* Если не удалось загрузить данные из файла по умолчанию*/
	{
		/* Спрашиваем пользователя о дальнейшем действии */
		int userChoice = warningMessage("Ошибка загрузки", "Не удалось загрузить данные из исходного файла.\nВыбрать другой файл для загрузки или запустить программу с исходными данными?");

		if (userChoice == 0)
		{
			/* Спрашиваем у пользователя имя файла */
			userFileName = QFileDialog::getOpenFileName(this, QString("Загрузить файл с данными"),
																QString(),
																	QString("Файлы Rent Something (*.rsd)"));
			/* Пытаемся загрузить пользовательский файл */
			if (!userFileName.isEmpty())
				isLoaded = load(userFileName);
		}
	}
	return isLoaded;
}

bool RentCalendar::save(QString &fileName)
{
	/* Создаем файл на основе имени */
	QFile MyFile(fileName);

	if (MyFile.open(QIODevice::WriteOnly))
	/* Если файл успешно открыт */
	{
		/* Создаем поток */
		QDataStream stream(&MyFile);
		/* Переносим данные в поток */
		stream << maxId << statusDirectory << itemDirectory << resDirectory;
		/* Закрываем файл */
		MyFile.close();
		return true;
	}
	return false;
}

bool RentCalendar::load(QString &fileName)
{
	/* Создаем файл на основе имени */
	QFile MyFile(fileName);

	if (MyFile.open(QIODevice::ReadOnly))
	/* Если файл успешно открыт */
	{
		/* Создаем поток */
		QDataStream stream(&MyFile);
		/* Вышружаем данные из потока */
		stream >> maxId >> statusDirectory >> itemDirectory >> resDirectory;
		/* Закрываем файл */
		MyFile.close();
		/* Возвращаем признак успеха операции */
		return true;
	}
	return false;
}

int RentCalendar::warningMessage(QString title, QString text)
{
	QMessageBox warning(QMessageBox::Question, title, text);
	warning.addButton("Выбрать другой файл...", QMessageBox::YesRole);
	warning.addButton("Запустить программу с исходными данными", QMessageBox::NoRole);
	return warning.exec();
}
