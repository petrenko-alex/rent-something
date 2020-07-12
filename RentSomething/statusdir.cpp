#include "statusdir.h"

StatusDir::StatusDir(QVector<Status> *sd,QVector<Reservation> *rd)
{
	ui.setupUi(this);
	QWidget::setWindowModality(Qt::ApplicationModal);

	/* Принимаем текущую базу состояний и резерваций */
	this->statusDirectory = sd;
	this->reservationDirectory = rd;

	/* Создаем форму добавления состояния */
	statusAdd = new StatusAdd(statusDirectory);

	/* Соединение слотов и сигналов */
	connections();

	/* Первоначальное состояние виджетов */
	ui.statusTable->clearSelection();
	ui.editStatusB->setEnabled(false);
	ui.deleteStatusB->setEnabled(false);
	ui.copyStatusB->setEnabled(false);
}

StatusDir::~StatusDir()
{
}

void StatusDir::intitializeForm()
{
	int statusNumber = statusDirectory->size();
	for (int i = 0; i < statusNumber; ++i)
	{
		addTableRow((*statusDirectory)[i]);
	}
	ui.statusTable->item(0, 0)->setToolTip("Это состояние по умолчанию.\nПредназначено для освобождения товара от резервации.\nВы не можете его изменить.");
}

void StatusDir::addDefaultStatus()
{
	/* Состояние по-умолчанию */
	QColor white("white");
	Status defaultStatus("Свободен", white);
	*statusDirectory << defaultStatus;
	addTableRow(defaultStatus);
	ui.statusTable->item(0, 0)->setToolTip("Это состояние по умолчанию.\nПредназначено для освобождения товара от резервации.\nВы не можете его изменить.");
}

void StatusDir::openStatusAdd()
{
	/* Настройка формы */
	statusAdd->setWindowTitle("Состояние / Добавить");
	statusAdd->ui.statusName->setText("");
	statusAdd->currentColor.setNamedColor("white");
	statusAdd->showColor();
	statusAdd->currentMode = ADD;
	/* Показ формы */
	statusAdd->show();
	statusAdd->ui.statusName->setFocus();
}

void StatusDir::openStatusCopy()
{
	/* Номер текущего ряда */ 
	int curRow = ui.statusTable->currentRow();
	/* Названия состояния в текущем ряду */
	QString statusName = ui.statusTable->item(curRow, 0)->data(0).toString();
	/* Индекс состояния в векторе */
	int statusNumber = findStatusIndex(statusName);
	/* Настройка формы */
	statusAdd->setWindowTitle("Состояние / Копировать");
	statusAdd->ui.statusName->setText(statusName);
	statusAdd->currentColor = (*statusDirectory)[statusNumber].getColor();
	statusAdd->showColor();
	statusAdd->currentMode = ADD;
	/* Показ формы */
	statusAdd->show();
}

void StatusDir::openStatusEdit()
{
	/* Номер текущего ряда */
	int curRow = ui.statusTable->currentRow();
	/* Отсеять попытку изменения статуса по умолчанию */
	if (ui.statusTable->item(curRow, 0)->text() == "Свободен")
		return;
	/* Названия состояния в текущем ряду */
	QString statusName = ui.statusTable->item(curRow, 0)->data(0).toString();
	/* Индекс состояния в векторе */
	int statusNumber = findStatusIndex(statusName);
	if (isUsedInCalendar((*statusDirectory)[statusNumber]))
	/* Нельзя изменить использующееся состояние */
	{
		QMessageBox::warning(this, "Проверка возможности изменения", "Это состояние используется в расписании.\n\nИзменение запрещено!");
		return;
	}
	/* Настройка формы */
	statusAdd->setWindowTitle("Состояние / Изменить");
	statusAdd->ui.statusName->setText(statusName);
	statusAdd->currentColor = (*statusDirectory)[statusNumber].getColor();
	statusAdd->showColor();
	statusAdd->currentMode = EDIT;
	/* Показ формы */
	statusAdd->show();
}

void StatusDir::closeStatusDir()
{
	this->close();
}

void StatusDir::addNewStatus(Status newStatus)
{
	/* Добавляем новое состояние в вектор и таблицу */
	*statusDirectory << newStatus;
	addTableRow(newStatus);
}

void StatusDir::editExistingStatus(Status newStatus)
{
	/* Номер текущего ряда */
	int curRow = ui.statusTable->currentRow();
	/* Название состояния в текущем ряду */
	QString statusName = ui.statusTable->item(curRow, 0)->data(0).toString();
	/* Индекс состояния в векторе */
	int statusNumber = findStatusIndex(statusName);
	/* Новые значения в векторе */
	(*statusDirectory)[statusNumber].setName(newStatus.getName());
	(*statusDirectory)[statusNumber].setColor(newStatus.getColor());
	/* Новые значения в таблице */
	ui.statusTable->item(curRow, 0)->setText(newStatus.getName());
}

void StatusDir::deleteStatus()
{
	/* Номер текущего ряда */
	int curRow = ui.statusTable->currentRow();
	/* Название состояния в текущем ряду */
	QString statusName = ui.statusTable->item(curRow, 0)->data(0).toString ();
	/* Индекс состояния в векторе */
	int statusNumber = findStatusIndex(statusName);
	if (statusNumber == 0)
	/* Нельзя удалить состояние по умолчанию */
	{
		QMessageBox::warning(this, "Проверка возможности удаления", "Эта строка - состояние \"по умолчанию\".\n\nУдаление запрещено!");
	}
	else if (isUsedInCalendar((*statusDirectory)[statusNumber]))
	/* Нельзя удалить использующееся состояние */
	{
		QMessageBox::warning(this, "Проверка возможности удаления", "Это состояние используется в расписании.\n\nУдаление запрещено!");
	}
	else if (statusNumber != -1)
	{
		(*statusDirectory).remove(statusNumber);
		deleteTableRow(curRow);
	}
}

void StatusDir::findStatus(const QString &status)
{
	/* Очищаем выделение */
	clearItemBackgrounds();
	ui.statusTable->clearSelection();

	/* Получаем список подходящих ячеек */
	QList<QTableWidgetItem *> match = ui.statusTable->findItems(status, Qt::MatchStartsWith | Qt::MatchCaseSensitive);

	/* Если строка поиска не пуста */
	if (ui.statusSearch->text () != "")
	{
		/* Закрашиваем подходящие ячейки зеленым */
		for (auto it : match)
		{
			(*it).setBackground(Qt::green);
		}
	}
	
	/* Если найдено только одно совпадение - выделяем его */
	if (match.size() == 1)
	{
		(*match.at(0)).setSelected(true);
		ui.statusTable->setCurrentItem(match.at(0));
	}

}

void StatusDir::activateButtons()
{
	/* Активность кнопок */
	ui.editStatusB->setEnabled(true);
	ui.deleteStatusB->setEnabled(true);
	ui.copyStatusB->setEnabled(true);

	int curRow = ui.statusTable->currentRow();
	if (ui.statusTable->item (curRow,0)->text () == "Свободен")
	{
		ui.editStatusB->setEnabled(false);
		ui.deleteStatusB->setEnabled(false);
		ui.copyStatusB->setEnabled(false);
	}
}

void StatusDir::connections()
{
	connect(ui.statusTable, SIGNAL(itemSelectionChanged()), SLOT(activateButtons()));
	connect(ui.addStatusB, SIGNAL(clicked(bool)), SLOT(openStatusAdd()));
	connect(ui.copyStatusB, SIGNAL(clicked(bool)), SLOT(openStatusCopy()));
	connect(ui.editStatusB, SIGNAL(clicked(bool)), SLOT(openStatusEdit()));
	connect(ui.statusTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(openStatusEdit()));
	connect(ui.exitStatusB, SIGNAL(clicked(bool)), SLOT(closeStatusDir()));
	connect(ui.deleteStatusB, SIGNAL(clicked(bool)), SLOT(deleteStatus()));
	connect(ui.statusSearch, SIGNAL(textChanged(const QString&)), SLOT(findStatus(const QString&)));
	connect(statusAdd, SIGNAL(newStatus(Status)), this, SLOT(addNewStatus(Status)));
	connect(statusAdd, SIGNAL(editStatus(Status)), this, SLOT(editExistingStatus(Status)));
}

void StatusDir::addTableRow(Status &newRow)
{
	QTableWidgetItem *item;
	int rowCount = ui.statusTable->rowCount();
	/* Вставляем ряд в таблицу */
	ui.statusTable->insertRow(rowCount);
	/* Создаем ячейку */
	item = new QTableWidgetItem(newRow.getName());
	ui.statusTable->setItem(rowCount, 0, item);
	/* Настройки таблицы */
	ui.statusTable->setCurrentCell(rowCount, 0);
	ui.statusTable->setRowCount(++rowCount);
}

void StatusDir::deleteTableRow(int rowNumber)
{
	/* Освобождаем память под ячейку */
	delete ui.statusTable->takeItem(rowNumber, 0);
	/* Удаляем ряд из таблицы */
	ui.statusTable->removeRow(rowNumber);
	/* Новое количество рядов */
	int rowCount = ui.statusTable->rowCount();
	ui.statusTable->setRowCount(rowCount);
}

int StatusDir::findStatusIndex(QString &statusName)
{
	int i = 0;
	/* Проход по вектору */
	for (auto it : *statusDirectory)
	{
		/* Если имя совпало */
		if (it.getName() == statusName)
			return i;
		++i;
	}
	return -1;
}

void StatusDir::clearItemBackgrounds()
{
	int rowCount = ui.statusTable->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		ui.statusTable->item(i, 0)->setBackground(Qt::white);
	}
}

bool StatusDir::isUsedInCalendar(const Status &status)
{
	for (auto it : *reservationDirectory)
	{
		if (it.status == status)
			return true;
	}
	return false;
}
