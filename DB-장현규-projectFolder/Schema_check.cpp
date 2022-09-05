#include "Schema_check.h"
#include "DatabaseAssignment.h"

Schema_check::Schema_check(QWidget *parent)
	: QWidget(parent)
{
	check.setupUi(this);
	QMessageBox msg;
	connect(check.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(get_schema()));
	Database* d = new Database();
	if (!d->DBConnect()) {
	}
	int col;
	char colName[100], colType[100];
	SQLCHAR query[100];
	string key;
	cout << check.comboBox->currentText().toStdString().c_str() << endl;
	SQLHSTMT hstmt;
	char Query[100];
	char type[100];
	if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hstmt) == SQL_SUCCESS) {
		sprintf((char*)query, "select * from %s", check.comboBox->currentText().toStdString().c_str());
		SQLExecDirect(hstmt,(SQLCHAR*) query, SQL_NTS);
		SQLNumResultCols(hstmt, (SQLSMALLINT*)&col);
		cout << "A" << endl;
		cout << col << endl;
		for (int i = 0; i < col; i++) {
			SQLDescribeCol(hstmt, i + 1, (SQLCHAR*)&colName[0], 100, NULL,  NULL, NULL,NULL, NULL);
			SQLColAttribute(hstmt, i + 1, SQL_DESC_TYPE_NAME, type, 100, NULL, NULL);
			key = key + string(colName)+"("+string(type)+")";
			if (i + 1 != col) {
				key = key + ",";
			}

		}
		check.schema->setText(key.c_str());
		SQLCloseCursor(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		d->DBDisconnect();
	}
}

Schema_check::~Schema_check()
{
}
void Schema_check::get_schema() {
	Database* d = new Database();
	if (!d->DBConnect()) {
		return;
	}
	int col;
	char colName[100], colType[100];
	SQLCHAR query[100];
	string key;
	cout << check.comboBox->currentText().toStdString().c_str() << endl;
	SQLHSTMT hstmt;
	char Query[100];
	char type[100];
	key = " ";
	if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hstmt) == SQL_SUCCESS) {
		sprintf((char*)query, "select * from %s", check.comboBox->currentText().toStdString().c_str());
		SQLExecDirect(hstmt,(SQLCHAR*) query, SQL_NTS);
		SQLNumResultCols(hstmt, (SQLSMALLINT*)&col);
		for (int i = 0; i < col; i++) {
			SQLDescribeCol(hstmt, i + 1, (SQLCHAR*)&colName[0], 100, NULL, NULL, NULL, NULL, NULL);
			SQLColAttribute(hstmt, i + 1, SQL_DESC_TYPE_NAME, type, 100, NULL, NULL);
			key = key + string(colName) + "(" + string(type) + ")";
			if (i + 1 != col) {
				key = key + ",";
			}
			cout << key << endl;
		}
		check.schema->setText(key.c_str());
		SQLCloseCursor(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
		d->DBDisconnect();
	}
}