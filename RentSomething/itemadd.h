#ifndef ITEMADD_H
#define ITEMADD_H

#include <QWidget>
#include "ui_itemadd.h"
#include "item.h"
#include "statusadd.h"



class ItemAdd : public QWidget
{
	Q_OBJECT

private slots:
	/* Нажатие на кнопку "ОК" */
	void okButtonClicked();

	/* Нажатие на кнопку "Отмена" */
	void cancelButtonClicked();

signals:
	/* Сигнал добавления нового наименования */
	void newItem(Item newItem);

	/* Сигнал изменения существующего наименования */
	void editItem(Item newItem);

private:
	/* Соединение сигналов и слотов */
	void connections();
	/* Проверка, существует ли  наименование с таким номером */
	bool ifItemExist();

public:
	ItemAdd(QVector<Item> *idir);
	~ItemAdd();

	Ui::ItemAdd ui;
	QVector<Item> *idir;			// Указатель на базу наименований
	mode currentMode;			// Текущий режим
};

#endif // ITEMADD_H
