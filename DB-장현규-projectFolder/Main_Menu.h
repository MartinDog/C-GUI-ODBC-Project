#pragma once

#include <QWidget>
#include "ui_Main_Menu.h"

class Main_Menu : public QWidget
{
	Q_OBJECT

public:
	Main_Menu(QWidget *parent = Q_NULLPTR);
	~Main_Menu();

private:
	Ui::Main_Menu ui;
public slots:
	void open_schema();
	void open_custom();
	void open_Select();
	void open_delete();
	void open_update();
	void open_insert();
};
