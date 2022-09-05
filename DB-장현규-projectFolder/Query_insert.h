#pragma once

#include <QWidget>
#include "ui_Query_insert.h"
#include"DatabaseAssignment.h"
class Query_insert : public QWidget
{
	Q_OBJECT

public:
	Query_insert(QWidget *parent = Q_NULLPTR);
	~Query_insert();

private:
	Ui::Query_insert ui;
public slots:
	void exec_query();
};
