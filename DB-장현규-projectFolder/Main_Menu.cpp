#include "Main_Menu.h"
#include"Schema_check.h"
#include"Custom_query.h"
#include"Query_insert.h"
#include"update_query.h"
#include"delete_tuple.h"
Main_Menu::Main_Menu(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this); 
	connect(ui.schema, SIGNAL(clicked(bool)), this, SLOT(open_schema()));
	connect(ui.custom, SIGNAL(clicked(bool)), this, SLOT(open_custom()));
	connect(ui.select, SIGNAL(clicked(bool)), this, SLOT(open_Select()));
	connect(ui.delete_2, SIGNAL(clicked(bool)), this, SLOT(open_delete()));
	connect(ui.insert, SIGNAL(clicked(bool)), this, SLOT(open_insert()));
	connect(ui.update, SIGNAL(clicked(bool)), this, SLOT(open_update()));
}
void Main_Menu::open_schema() {
	Schema_check* sch = new Schema_check();
	sch->show();
}void Main_Menu::open_custom() {
	Custom_query* custom = new Custom_query();
	custom->show();
}
void Main_Menu::open_Select() {
	DatabaseAssignment* db = new DatabaseAssignment();
	db->show();
}
void Main_Menu::open_delete() {
	delete_tuple* db = new delete_tuple();
	db->show();
}
void Main_Menu::open_insert() {
	Query_insert* db = new Query_insert();
	db->show();
}
void Main_Menu::open_update() {
	update_query* q = new update_query();
	q->show();
}
Main_Menu::~Main_Menu()
{
}
