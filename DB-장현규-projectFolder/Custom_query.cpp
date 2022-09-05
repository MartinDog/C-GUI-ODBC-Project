#include "Custom_query.h"
#include "DatabaseAssignment.h"
#include<string>
Custom_query::Custom_query(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableView->setVisible(false);
	ui.insert->setChecked(true);
	connect(ui.excute, SIGNAL(clicked(bool)), this, SLOT(exec_query()));

}

Custom_query::~Custom_query()
{
}
void Custom_query::exec_query() {
	Database* d = new Database();
	if (!d->DBConnect()) {
		return;
	}
	SQLHSTMT hstmt;
	if (strlen(ui.query->toPlainText().toLocal8Bit().toStdString().c_str()) == 0) {
		return;
	}
	if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hstmt) == SQL_SUCCESS) {
		string query = ui.query->toPlainText().toLocal8Bit().toStdString().c_str();
		cout << (SQLCHAR*)query.c_str() << endl;
		if (SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS) == SQL_SUCCESS) {
			if (ui.insert->isChecked()) {
				ui.label_2->setVisible(false);
				ui.tableView->setVisible(true);
				int col;
				SQLNumResultCols(hstmt, (SQLSMALLINT*)&col);
				cout << col << endl;
				char** values = new char* [col];
				QStandardItemModel* EMP;
				EMP = new QStandardItemModel(0, col);
				SQLLEN* nulldata = new SQLLEN[col];
				for (int i = 0; i < col; i++) {
					values[i] = new char[100];
					SQLDescribeCol(hstmt, i + 1, (SQLCHAR*)values[i], 100, NULL, NULL, NULL, NULL, NULL);
					cout << i << " " << values[i] << endl;
					EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(values[i]).c_str()));
					SQLBindCol(hstmt, i + 1, SQL_C_CHAR, values[i], 100, nulldata + i);
				}
				int rowCount = 0;
				while (SQLFetch(hstmt) != SQL_NO_DATA) {
					string s;
					EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
					cout << s << endl;
					for (int j = 0; j < col; j++) {
						s = values[j];
						if (nulldata[j] == SQL_NULL_DATA) {
							EMP->setData(EMP->index(rowCount, j), "NULL");
						}
						else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));

					}
					rowCount++;
				}
				ui.tableView->setModel(EMP);
			}
			else {
				ui.label_2->setVisible(true);
				ui.tableView->setVisible(false);
			}
		}
		else {
			char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
			SQLSMALLINT length;
			SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
			cout << buffer << endl;
			QMessageBox msg;
			QString s(buffer);
			msg.setText(s);
			msg.exec();
		}
	}
	else {
		cout << "error" << endl;
	}
	d->DBDisconnect();
}
