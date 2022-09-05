#pragma once

#include <QWidget>
#include "ui_Custom_query.h"

class Custom_query : public QWidget
{
	Q_OBJECT

public:
	Custom_query(QWidget *parent = Q_NULLPTR);
	~Custom_query();

private:
	Ui::Custom_query ui;
public slots:
	void exec_query();
};
