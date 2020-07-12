#ifndef STATUSDIR_H
#define STATUSDIR_H

#include <QWidget>
#include "ui_statusdir.h"
#include "statusadd.h"
#include "reservation.h"

class StatusDir : public QWidget
{
	Q_OBJECT

public:
	StatusDir(QVector<Status> *sd, QVector<Reservation> *rd);
	~StatusDir();

	/* Инициализация формы при успешной загрузке из файла */
	void intitializeForm();
	/* Добавить состояние по умолчанию */
	void addDefaultStatus();

private slots:
	/* Нажатие кнопки "Добавить" */
	void openStatusAdd();

	/* Нажатие кнопки "Копировать" */
	void openStatusCopy();

	/* Нажатие кнопки "Изменить" */
	void openStatusEdit();

	/* Нажатие кнопки "Выход" */
	void closeStatusDir();

	/* Добавление нового состояния в вектор по сигналу из формы statusAdd */
	void addNewStatus(Status newStatus);

	/* Изменение существующего состояния в векторе по сигналу из формы statusAdd */
	void editExistingStatus(Status newStatus);

	/* Нажатие кнопки "Удалить" */
	void deleteStatus();

	/* Поиск состояния через строку поиска */
	void findStatus(const QString &status);

	/* Активация кнопок "Копировать" "Изменить" и "Удалить" */
	void activateButtons();

private:
	/* Соединение сигналов и слотов */
	void connections();

	/* Добавление ряда в таблицу */
	void addTableRow(Status &newRow);

	/* Удаление ряда из таблицы */
	void deleteTableRow(int rowNumber);

	/* Поиск индекса состояния в векторе по имени */
	int  findStatusIndex(QString &statusName);

	/* Очистка фона рядов в таблице */
	void clearItemBackgrounds();

	/* Проверка, используется ли данное состояние в календаре */
	bool isUsedInCalendar(const Status &status);

	Ui::StatusDir ui;
	StatusAdd *statusAdd;								// Форма statusAdd
	QVector<Status> *statusDirectory;					// Вектор состояний
	QVector<Reservation> *reservationDirectory;			// Вектор резерваций
};

#endif // STATUSDIR_H
