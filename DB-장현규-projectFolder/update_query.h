#pragma once

#include <QWidget>
#include "ui_update_query.h"

class update_query : public QWidget
{
	Q_OBJECT

public:
	update_query(QWidget *parent = Q_NULLPTR);
	~update_query();

private:
	Ui::update_query ui;
public slots:
	void execution();
	void change_value(QString s);
};
