#ifndef RENTCALENDAR_H
#define RENTCALENDAR_H

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QFile>
#include "ui_rentcalendar.h"
#include "itemdir.h"
#include "statusdir.h"
#include "status.h"
#include "item.h"
#include "reservation.h"
#include "rentadd.h"
#define  MAXCOLUMN 35


class RentCalendar : public QMainWindow
{
	Q_OBJECT

public:
	RentCalendar(QWidget *parent = 0);
	~RentCalendar();

	/* Перегрузка события закрытия окна */
	void closeEvent(QCloseEvent *event);

private:
	/* Соединение сигналов и слотов */
	void connections();
	/* Первоначальная инициализация календаря при старте */
	void initializeCalendar();
	/* Добавить ряд в календарь */
	void addCalendarRow(Item &newRow);
	/* Добавить ячейку в календарь */
	void addCalendarItem(int row,int column,Reservation &newRes);
	/* Удалить ячейку из календаря */
	void delCalendatItem(int row, int column);
	/* Найти номер колонки в календаре по дате */
	int findColumnByDate(const QDate &date);
	/* Найти индекс наименования по номеру */
	int findItemIndexByNumber(const QString &number);
	/* Найти запись индекс записи о резервации в векторе по id */
	int findResIndexById(const unsigned int id);
	/* Найти номер ряда в календаре по номеру наименования */
	int findRowNumber(const QString &itemNumber);
	/* Добавить запись в календарь */
	void addReservationToCalendar(Reservation &newRes);
	/* Удалить запись из календаря */
	void delReservatrionFromCalendar(Reservation &oldRes);
	/* Сформировать колонки календаря на основе начальной даты на месяц вперед */
	void formMonthCalendarColumns(const QDate &startDate);
	/* Удалить из текущего календаря ячейки с резервациями */
	void deleteResItems();
	/* Удалить из текущего календаря заголовки с датами */
	void deleteCalendarHeaderItems();
	/* Добавить в текущий календарь ячейки с резервациями */
	void addResItems();
	/* Перестроить календарь по значениям curCalendarStart и curCalendarEnd */
	void rebuildCurCalendar();
	/* Удалить ряды в календаре(с 1 по 3 колонки) */
	void deleteCalendarRows();
	/* Построить ряды календаря на основе вектора наименований */
	void buildCalendarRows();

private slots:
	/* Нажате на кнопку "Справочник наименований" */
	void openItemDirectory();
	/* Нажате на кнопку "Справочник состояний" */
	void openStatusDirectory();
	/* Нажате на кнопку "OK" */
	void closeProgram();
	/* Нажатие на одну из ячеек календаря */
	void openResAdd(int row, int column);
	/* Добавление новой записи резервации */
	void addNewRes(Reservation &newRes);
	/* Редактирование существующей записи резервации */
	void editExistingRes(Reservation &existingRes);
	/* Перемещение по календарю - следующий день */
	void nextDay();
	/* Перемещение по календарю - предыдущий день */
	void prevDay();
	/* Перемещение по календарю - следующий месяц */
	void nextMonth();
	/* Перемещение по календарю - предыдущий месяц */
	void prevMonth();
	/* Перемещение по календарю - изменение даты в поле "Дата начала" */
	void dateChanged();
	/* Показ информации в полях снизу при клике на ячейку */
	void showBriefInfo(int row,int column);
	/* Обновление календаря, при внесении изменений в справочник наименований */
	void refreshCalendar(bool itemDirChanged);
	/* Сохранить данные программы в файл */
	void saveData();
	/* Загрузить данные программы из файла */
	bool loadData();
	/* Вспомогательная функция сохранения */
	bool save(QString &fileName);
	/* Вспомогательная функция загрузки */
	bool load(QString &fileName);
	/* Предупреждающее сообщение */
	int warningMessage(QString title, QString text);

private:
	Ui::RentCalendarClass ui;
	ItemDir		*itemDir;									// Форма справочника наименований
	StatusDir	*statusDir;									// Форма справочника состояний
	RentAdd		*rentAdd;									// Форма добавления новой резервации
	QString defaultFileName;
	QString userFileName;

	QVector<Status>		 statusDirectory;					// Вектор состояний
	QVector<Item>		 itemDirectory;						// Вектор наименований
	QVector<Reservation> resDirectory;						// Вектор резерваций
	unsigned int		 maxId;								// Максимальный ID в resDirectory
	QDate curCalendarStart;
	QDate curCalendarEnd;
	bool blockDateChangedSignal;

public:
	struct calendarItem										// Структура ячейки календаря
	{
		Status status;										// Текущий статус
		int resId;										    // ID записи резервации в resDirectory
	};
};

Q_DECLARE_METATYPE(RentCalendar::calendarItem);

#endif // RENTCALENDAR_H
