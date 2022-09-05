#pragma once

#include <QWidget>
#include "ui_DELETE.h"

class DELETE : public QWidget
{
	Q_OBJECT

public:
	DELETE(QWidget *parent = Q_NULLPTR);
	~DELETE();
	

private:
	Ui::DELETE ui;
};
