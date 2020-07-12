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

/* ����� ������ �����: ���������� ������ ��������� ��� ��������� ������������� */
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
	/* ������� �� ������ "��" */
	void okButtonClicked();

	/* ������� �� ������ "������" */
	void cancelButtonClicked();

	/* ������� �� ������ "�������" */
	void selectColor();

signals:
	/* ������ ���������� ������ ��������� */
	void newStatus(Status newStatus);
	
	/* ������ ��������� ������������� ��������� */
	void editStatus(Status newStatus);

private:
	/* ���������� �������� � ������ */
	void connections();

	/* ��������, ���������� �� ��������� � ����� �������� */
	bool ifStatusExist();

public:
	/* ����� ���������� ����� � QLabel ����� � ������� "�������" */
	void showColor();

	Ui::StatusAdd ui;
	QColor currentColor;			// ������� ����
	QVector<Status> *sd;			// ��������� �� ���� ���������
	mode currentMode;				// ������� �����
};

#endif // STATUSADD_H
