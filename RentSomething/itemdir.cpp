#include "itemdir.h"

ItemDir::ItemDir(QVector<Item> *idir, QVector<Reservation> *rd)
{
	ui.setupUi(this);
	QWidget::setWindowModality(Qt::ApplicationModal);

	/* Принимаем текущую базу наименований и резерваций*/
	this->itemDirectory = idir;
	this->reservationDirectory = rd;
	
	/* Создаем форму добавления наименования */
	itemAdd = new ItemAdd(itemDirectory);
	
	/* Соединение слотов и сигналов */
	connections();

	/* Первоначальное состояние виджетов */
	ui.itemTable->clearSelection();
	ui.editItemB->setEnabled(false);
	ui.deleteItemB->setEnabled(false);
	ui.copyItemB->setEnabled(false);

	/* Первоначальная инициализация переменных */
	itemDirectoryChanged = false;
}

ItemDir::~ItemDir()
{
}

void ItemDir::closeEvent(QCloseEvent *event)
{
	emit itemDirClosed(itemDirectoryChanged);
}

void ItemDir::intitializeForm()
{
	int itemNumber = itemDirectory->size();
	for (int i = 0; i < itemNumber; ++i)
	{
		addTableRow((*itemDirectory)[i]);
	}
}

void ItemDir::openItemAdd()
{
	/* Настройка формы */
	itemAdd->setWindowTitle("Наименование / Добавить");
	itemAdd->ui.itemName->setText("");
	itemAdd->ui.itemNumber->setText("");
	itemAdd->ui.itemPrice1->setValue(1);
	itemAdd->ui.itemPrice2->setValue(1);
	itemAdd->ui.itemPrice3->setValue(1);
	itemAdd->ui.itemPrice4->setValue(1);
	itemAdd->ui.itemPrice5->setValue(1);
	itemAdd->currentMode = ADD;
	/* Показ формы */
	itemAdd->show();
	itemAdd->ui.itemName->setFocus();
}

void ItemDir::openItemCopy()
{
	/* Номер текущего ряда */
	int curRow = ui.itemTable->currentRow();
	/* Номер наименования в текущем ряду */
	QString itemNumber = ui.itemTable->item(curRow, 1)->data(0).toString();
	/* Индекс наименования в векторе */
	int itemIndex = findItemIndex(itemNumber);
	/* Настройка формы */
	itemAdd->setWindowTitle("Наименование / Копировать");
	itemAdd->ui.itemName->setText((*itemDirectory)[itemIndex].getName());
	itemAdd->ui.itemNumber->setText((*itemDirectory)[itemIndex].getNumber());
	itemAdd->ui.itemPrice1->setValue((*itemDirectory)[itemIndex].getPrices()[0]);
	itemAdd->ui.itemPrice2->setValue((*itemDirectory)[itemIndex].getPrices()[1]);
	itemAdd->ui.itemPrice3->setValue((*itemDirectory)[itemIndex].getPrices()[2]);
	itemAdd->ui.itemPrice4->setValue((*itemDirectory)[itemIndex].getPrices()[3]);
	itemAdd->ui.itemPrice5->setValue((*itemDirectory)[itemIndex].getPrices()[4]);
	itemAdd->currentMode = ADD;
	/* Показ формы */
	itemAdd->show();
}

void ItemDir::openItemEdit()
{
	/* Номер текущего ряда */
	int curRow = ui.itemTable->currentRow();
	/* Номер наименования в текущем ряду */
	QString itemNumber = ui.itemTable->item(curRow, 1)->data(0).toString();
	/* Индекс наименования в векторе */
	int itemIndex = findItemIndex(itemNumber);
	if (isUsedInCalendar((*itemDirectory)[itemIndex]))
	/* Нельзя удалить использующееся состояние */
	{
		QMessageBox::warning(this, "Проверка возможности изменения", "Это наименование используется в расписании.\n\nИзменение запрещено!");
		return;
	}
	/* Настройка формы */
	itemAdd->setWindowTitle("Наименование / Изменить");
	itemAdd->ui.itemName->setText((*itemDirectory)[itemIndex].getName());
	itemAdd->ui.itemNumber->setText((*itemDirectory)[itemIndex].getNumber());
	itemAdd->ui.itemPrice1->setValue((*itemDirectory)[itemIndex].getPrices()[0]);
	itemAdd->ui.itemPrice2->setValue((*itemDirectory)[itemIndex].getPrices()[1]);
	itemAdd->ui.itemPrice3->setValue((*itemDirectory)[itemIndex].getPrices()[2]);
	itemAdd->ui.itemPrice4->setValue((*itemDirectory)[itemIndex].getPrices()[3]);
	itemAdd->ui.itemPrice5->setValue((*itemDirectory)[itemIndex].getPrices()[4]);
	itemAdd->currentMode = EDIT;
	/* Показ формы */
	itemAdd->show();
	
}

void ItemDir::deleteItem()
{
	/* Номер текущего ряда */
	int curRow = ui.itemTable->currentRow();
	/* Номер наименования в текущем ряду */
	QString itemNumber = ui.itemTable->item(curRow, 1)->data(0).toString();
	/* Индекс наименования в векторе */
	int itemIndex = findItemIndex(itemNumber);
	if (isUsedInCalendar((*itemDirectory)[itemIndex]))
	/* Нельзя удалить использующееся состояние */
	{
		QMessageBox::warning(this, "Проверка возможности удаления", "Это наименование используется в расписании.\n\nУдаление запрещено!");
	}
	else if (itemIndex != -1)
	{
		(*itemDirectory).remove(itemIndex);
		deleteTableRow(curRow);
		/* Устанавливаем признак изменения справочника наименований */
		itemDirectoryChanged = true;
	}

	/* Деактивация кнопок если таблица пуста*/
	if (ui.itemTable->rowCount() == 0)
	{
		deactivateButtons();
	}
}

void ItemDir::closeItemDir()
{
	this->close();
}

void ItemDir::addNewItem(Item newItem)
{
	/* Добавляем новое наименование в вектор и таблицу */
	*itemDirectory << newItem;
	addTableRow(newItem);
	/* Устанавливаем признак изменения справочника наименований */
	itemDirectoryChanged = true;
}

void ItemDir::editExistingItem(Item newItem)
{
	/* Номер текущего ряда */
	int curRow = ui.itemTable->currentRow();
	/* Номер наименования в текущем ряду */
	QString itemNumber = ui.itemTable->item(curRow, 1)->data(0).toString();
	/* Индекс наименования в векторе */
	int itemIndex = findItemIndex(itemNumber);
	/* Новые значения в векторе */
	(*itemDirectory)[itemIndex].setName(newItem.getName());
	(*itemDirectory)[itemIndex].setNumber(newItem.getNumber());
	(*itemDirectory)[itemIndex].setPrices(newItem.getPrices());
	/* Новые значения в таблице */
	ui.itemTable->item(curRow, 0)->setText(newItem.getName());
	ui.itemTable->item(curRow, 1)->setText(newItem.getNumber());
	ui.itemTable->item(curRow, 2)->setText(QString::number(newItem.getPrices()[0]));
	/* Устанавливаем признак изменения справочника наименований */
	itemDirectoryChanged = true;
}

void ItemDir::findItem(const QString &item)
{
	/* Очищаем выделение */
	clearItemBackgrounds();
	ui.itemTable->clearSelection();

	/* Получаем список подходящих ячеек */
	QList<QTableWidgetItem *> match = ui.itemTable->findItems(item, Qt::MatchStartsWith | Qt::MatchCaseSensitive);

	/* Если строка поиска не пуста */
	if (ui.itemSearch->text() != "")
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
		ui.itemTable->setCurrentItem(match.at(0));
	}
}

void ItemDir::activateButtons()
{
	/* Активность кнопок */
	if (ui.itemTable->rowCount() != 0)
	{
		ui.editItemB->setEnabled(true);
		ui.deleteItemB->setEnabled(true);
		ui.copyItemB->setEnabled(true);
	}
}

void ItemDir::deactivateButtons()
{
	ui.editItemB->setEnabled(false);
	ui.deleteItemB->setEnabled(false);
	ui.copyItemB->setEnabled(false);
}

void ItemDir::connections()
{
	connect(ui.itemTable, SIGNAL(itemSelectionChanged()), SLOT(activateButtons()));
	connect(ui.addItemB, SIGNAL(clicked(bool)), SLOT(openItemAdd()));
	connect(ui.copyItemB, SIGNAL(clicked(bool)), SLOT(openItemCopy()));
	connect(ui.editItemB, SIGNAL(clicked(bool)), SLOT(openItemEdit()));
	connect(ui.itemTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(openItemEdit()));
	connect(ui.exitItemB, SIGNAL(clicked(bool)), SLOT(closeItemDir()));
	connect(ui.deleteItemB, SIGNAL(clicked(bool)), SLOT(deleteItem()));
	connect(ui.itemSearch, SIGNAL(textChanged(const QString&)), SLOT(findItem(const QString&)));
	connect(itemAdd, SIGNAL(newItem(Item)), this, SLOT(addNewItem(Item)));
	connect(itemAdd, SIGNAL(editItem(Item)), this, SLOT(editExistingItem(Item)));
}

void ItemDir::addTableRow(Item &newRow)
{
	QTableWidgetItem *item;
	int rowCount = ui.itemTable->rowCount();
	/* Вставляем ряд в таблицу */
	ui.itemTable->insertRow(rowCount);
	/* Создаем 1-ю ячейку */
	item = new QTableWidgetItem(newRow.getName());
	ui.itemTable->setItem(rowCount, 0, item);
	/* Создаем 2-ю ячейку */
	item = new QTableWidgetItem(newRow.getNumber());
	ui.itemTable->setItem(rowCount, 1, item);
	/* Создаем 3-ю ячейку */
	item = new QTableWidgetItem(QString::number(newRow.getPrices()[0]));
	ui.itemTable->setItem(rowCount, 2, item);
	/* Настройки таблицы */
	ui.itemTable->setCurrentCell(rowCount, 0);
	ui.itemTable->setRowCount(++rowCount);
}

void ItemDir::deleteTableRow(int rowNumber)
{
	/* Освобождаем память под ячейки */
	delete ui.itemTable->takeItem(rowNumber, 0);
	delete ui.itemTable->takeItem(rowNumber, 1);
	delete ui.itemTable->takeItem(rowNumber, 2);
	/* Удаляем ряд из таблицы */
	ui.itemTable->removeRow(rowNumber);
	/* Новое количество рядов */
	int rowCount = ui.itemTable->rowCount();
	ui.itemTable->setRowCount(rowCount);
}

int ItemDir::findItemIndex(QString &itemNumber)
{
	int i = 0;
	/* Проход по вектору */
	for (auto it : *itemDirectory)
	{
		/* Если номер совпал */
		if (it.getNumber() == itemNumber)
			return i;
		++i;
	}
	return -1;
}

void ItemDir::clearItemBackgrounds()
{
	int rowCount = ui.itemTable->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		ui.itemTable->item(i, 0)->setBackground(Qt::white);
		ui.itemTable->item(i, 1)->setBackground(Qt::white);
		ui.itemTable->item(i, 2)->setBackground(Qt::white);
	}
}

bool ItemDir::isUsedInCalendar(const Item &item)
{
	for (auto it : *reservationDirectory)
	{
		if (it.item == item)
			return true;
	}
	return false;
}
