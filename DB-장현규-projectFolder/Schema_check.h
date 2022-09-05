#pragma once
#include "DatabaseAssignment.h"
#include <QWidget>
#include "ui_Schema_check.h"

class Schema_check : public QWidget
{
	Q_OBJECT

public:
	Schema_check(QWidget *parent = Q_NULLPTR);
	~Schema_check();

private:
	Ui::Schema_check check;
public slots:
	void get_schema();
};
