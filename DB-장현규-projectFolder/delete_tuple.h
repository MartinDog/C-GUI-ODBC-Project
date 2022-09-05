#pragma once

#include <QWidget>
#include "ui_delete_tuple.h"
#include "DatabaseAssignment.h"
class delete_tuple : public QWidget
{
	Q_OBJECT

public:
	int current_index;
	delete_tuple(QWidget *parent = Q_NULLPTR);
	~delete_tuple();

private:
	Ui::delete_tuple uis;
public slots:
	void execute();
};
