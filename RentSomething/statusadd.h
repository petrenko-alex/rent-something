#ifndef STATUSADD_H
#define STATUSADD_H

#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QRect>
#include "ui_statusadd.h"
#include "status.h"

/* Режим работы формы: добавление нового состояния или изменение существующего */
enum mode
{
	ADD,
	EDIT
};

class StatusAdd : public QWidget
{
	Q_OBJECT

public:
	StatusAdd(QVector<Status> *sd);
	~StatusAdd();

private slots:
	/* Нажатие на кнопку "ОК" */
	void okButtonClicked();

	/* Нажатие на кнопку "Отмена" */
	void cancelButtonClicked();

	/* Нажатие на кнопку "Выбрать" */
	void selectColor();

signals:
	/* Сигнал добавления нового состояния */
	void newStatus(Status newStatus);
	
	/* Сигнал изменения существующего состояния */
	void editStatus(Status newStatus);

private:
	/* Соединение сигналов и слотов */
	void connections();

	/* Проверка, существует ли состояние с таким имененем */
	bool ifStatusExist();

public:
	/* Показ выбранного цвета в QLabel рядом с кнопкой "Выбрать" */
	void showColor();

	Ui::StatusAdd ui;
	QColor currentColor;			// Текущий цвет
	QVector<Status> *sd;			// Указатель на базу состояний
	mode currentMode;				// Текущий режим
};

#endif // STATUSADD_H
