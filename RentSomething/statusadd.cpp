#include "statusadd.h"

StatusAdd::StatusAdd(QVector<Status> *sd)
{
	ui.setupUi(this);
	QWidget::setWindowModality(Qt::ApplicationModal);
	/* Цвет по-умолчанию - белый */ 
	currentColor.setNamedColor("white");
	/* Принимаем текущую базу состояний */
	this->sd = sd;
	/* Режим по умолчанию - добавление новой записи */
	currentMode = ADD;
	/* Первоначальная инициализация */
	connections();
	showColor();
}


StatusAdd::~StatusAdd()
{

}

void StatusAdd::okButtonClicked()
{
	/* Создаем временное состояние с именем и цветом */
	Status ns(ui.statusName->text(), currentColor);
	
	/* Проверка на пустые поля */
	if (ui.statusName->text() == "")
	{
		/* Невозможно добавить состояние с пустым именем */
		QMessageBox::warning(this, "Ошибка добавления состояния", "Название состояния не введено.");
		return;
	}

	if (currentMode == ADD && !ifStatusExist())
	/* Если состояния с таким именем еще не существует */
	{
		/* Посылаем состояние в форму statusDir в режиме добавления */
		emit newStatus(ns);
		this->close();
	}
	else if (currentMode == EDIT)
	{
		/* Посылаем состояние в форму statusDir в режиме изменения */
		emit editStatus(ns);
		this->close();
	}
	else
	{
		/* Невозможно добавить состояние с повторяющимся именем */
		QMessageBox::warning(this, "Ошибка добавления состояния", "Состояние с текущим именем уже существует.\nВведите уникальное имя для нового состояния.");
	}
}

void StatusAdd::cancelButtonClicked()
{
	this->close();
}

void StatusAdd::selectColor()
{
	/* Вызов диалогового окна выбора цвета */
	currentColor = QColorDialog::getColor();
	if (!currentColor.isValid())
	/* Если пользователь не выбрал цвет */
	{
		QMessageBox::warning(this,"Цвет не быбран", "Будет установлен цвет по умолчанию - белый.", QMessageBox::Ok);
		currentColor.setNamedColor("white");
	}
	showColor();
}

void StatusAdd::connections()
{
	connect(ui.statusAddOkB, SIGNAL(clicked(bool)), SLOT(okButtonClicked()));
	connect(ui.statusAddCancelB, SIGNAL(clicked(bool)), SLOT(cancelButtonClicked()));
	connect(ui.statusColorB, SIGNAL(clicked(bool)), SLOT(selectColor()));
}

bool StatusAdd::ifStatusExist()
{
	for (auto it : *sd)
	{
		if (it.getName() == ui.statusName->text())
			return true;
	}
	return false;
}

void StatusAdd::showColor()
{
	/* Создаем Pixmap с размерами QLabel */
	QPixmap myPix(QSize(ui.statusColor->width() + 50, ui.statusColor->height()));
	QPainter painter(&myPix);
	/* Рисуем прямоугольник с размерами QLabel */
	QRect rect(0, 0, ui.statusColor->width() + 50, ui.statusColor->height());
	/* Заполняем прямоугольник текущим цветом */
	painter.fillRect(rect, currentColor);
	/* Вставляем Pixmap в QLabel */
	ui.statusColor->setPixmap(myPix);
}
