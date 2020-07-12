#ifndef ITEMDIR_H
#define ITEMDIR_H

#include <QWidget>
#include "ui_itemdir.h"
#include "itemadd.h"
#include "reservation.h"

class ItemDir : public QWidget
{
	Q_OBJECT

public:
	ItemDir(QVector<Item> *idir, QVector<Reservation> *rd);
	~ItemDir();

	/* Перегрузка события закрытия окна */
	void closeEvent(QCloseEvent *event);

	/* Инициализация формы при успешной загрузке из файла */
	void intitializeForm();

private slots:

	/* Нажатие кнопки "Добавить" */
	void openItemAdd();

	/* Нажатие кнопки "Копировать" */
	void openItemCopy();

	/* Нажатие кнопки "Изменить" */
	void openItemEdit();

	/* Нажатие кнопки "Удалить" */
	void deleteItem();

	/* Нажатие кнопки "Выход" */
	void closeItemDir();

	/* Добавление нового наименования в вектор по сигналу из формы itemAdd */
	void addNewItem(Item newItem);

	/* Изменение существующего наименования в векторе по сигналу из формы itemAdd */
	void editExistingItem(Item newItem);

	/* Поиск наименования через строку поиска */
	void findItem(const QString &item);

	/* Активация кнопок "Копировать" "Изменить" и "Удалить" */
	void activateButtons();

	/* Деактивация кнопок "Копировать" "Изменить" и "Удалить" когда таблица пуста */
	void deactivateButtons();

signals:
	/* Сигнал, испускающийся при закрытии формы справочника наименований */
	void itemDirClosed(bool isItemDirChanged);

private:

	/* Соединение сигналов и слотов */
	void connections();

	/* Добавление ряда в таблицу */
	void addTableRow(Item &newRow);

	/* Удаление ряда из таблицы */
	void deleteTableRow(int rowNumber);

	/* Поиск индекса состояния в векторе по имени */
	int  findItemIndex(QString &itemNumber);

	/* Очистка фона рядов в таблице */
	void clearItemBackgrounds();

	/* Проверка, используется ли данное состояние в календаре */
	bool isUsedInCalendar(const Item &item);

	Ui::ItemDir ui;
	ItemAdd *itemAdd;									// Форма itemAdd
	QVector<Item> *itemDirectory;						// Вектор наименований
	QVector<Reservation> *reservationDirectory;			// Вектор резерваций
	bool itemDirectoryChanged;							// Признак изменения справочника наименований(itemDirectory)
};

#endif // ITEMDIR_H
