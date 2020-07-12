#include "itemadd.h"

ItemAdd::ItemAdd(QVector<Item> *idir)
{
	ui.setupUi(this);
	QWidget::setWindowModality(Qt::ApplicationModal);

	/* Принимаем текущую базу наименований */
	this->idir = idir;
	/* Режим по умолчанию - добавление новой записи */
	currentMode = ADD;
	/* Первоначальная инициализация */
	connections();
}

ItemAdd::~ItemAdd()
{

}

void ItemAdd::okButtonClicked()
{
	/* Создаем временное наименование */
	QVector<float> prices;
	prices << ui.itemPrice1->value();
	prices << ui.itemPrice2->value();
	prices << ui.itemPrice3->value();
	prices << ui.itemPrice4->value();
	prices << ui.itemPrice5->value();
	Item ni(ui.itemName->text(), ui.itemNumber->text(), prices);

	/* Проверка на пустые поля */
	if (ui.itemName->text() == "")
	{
		/* Невозможно добавить наименование с пустым именем */
		QMessageBox::warning(this, "Ошибка добавления наименования", "Название наименования не введено.");
		return;
	}
	else if (ui.itemNumber->text() == "")
	{
		/* Невозможно добавить наименование с пустым номером */
		QMessageBox::warning(this, "Ошибка добавления наименования", "Уникальный номер наименования не введен.");
		return;
	}

	if (currentMode == ADD && !ifItemExist())
	/* Если наименование с таким номером еще не существует */
	{
		/* Посылаем наименование в форму itemDir в режиме добавления */
		emit newItem(ni);
		this->close();
	}
	else if (currentMode == EDIT)
	{
		/* Посылаем наименование в форму itemDir в режиме изменения */
		emit editItem(ni);
		this->close();
	}
	else
	{
		/* Невозможно добавить наименование с повторяющимся номером */
		QMessageBox::warning(this, "Ошибка добавления наименования", "Наименование с текущим номером уже существует.\nВведите уникальный номер для нового наименования.");
	}
}

void ItemAdd::cancelButtonClicked()
{
	this->close();
}

void ItemAdd::connections()
{
	connect(ui.itemAddOkB, SIGNAL(clicked(bool)), SLOT(okButtonClicked()));
	connect(ui.itemAddCancelB, SIGNAL(clicked(bool)), SLOT(cancelButtonClicked()));
}

bool ItemAdd::ifItemExist()
{
	for (auto it : *idir)
	{
		if (it.getNumber() == ui.itemNumber->text())
			return true;
	}
	return false;
}

