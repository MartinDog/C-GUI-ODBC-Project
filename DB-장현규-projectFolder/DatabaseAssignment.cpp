#include "DatabaseAssignment.h"
#include<vector>
#include<string>
bool Database::DBConnect() {
    SQLRETURN Ret;
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS) {//SQLALLocHandle(핸들의 타입 설정,입력에 사용된 설정, 값을 입력 받을 핸들 변수)
        cout << 1 << endl;
        
        return false;
    }
    if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS) {//sql 환경변수 설정 SQLSetEnvAttr(환경핸들의 아이디,속성값,ODBC의 버전및 속성, 3번째 인사의 크기)
        cout << 2 << endl;
        
        return false;
    }
    if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS) {//SQLALLocHandle(핸들의 타입 설정,입력에 사용된 설정, 값을 입력 받을 핸들 변수)
        cout << 3 << endl;
        
        return false;
    }
    Ret = SQLConnect(hDbc,(SQLCHAR*) "DB01_Server", SQL_NTS,(SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"1234", SQL_NTS);//SQLConnect(연결 핸들의 아이디, 서버이름, 두번째 인자의 크기.접속아이디,4번째인자의 크기, 비밀번호, 6번째 인사의 크기
    if ((Ret != SQL_SUCCESS) && (Ret != SQL_SUCCESS_WITH_INFO)) {

        cout << Ret<<endl;
        return false;
    }
    
    return true;
}
void Database::DBDisconnect()
{
    SQLDisconnect(hDbc);// Disconnect from the SQL Server
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);// Free the Connection Handle
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);// Free the Environment Handle
}


void DatabaseAssignment::initialize(string s) {
    d = new Database();
    QTableView *ta= new QTableView;
    if (d->DBConnect() == true) {
        static SQLCHAR query[110];
        SQLHSTMT hStmt;
        if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {

            char data[50][50];
            char colName[50][50];
            char Name[50];
            static char q[100];
            SQLLEN nulldata[50];
            SQLSMALLINT colCount = -1;
            int rowCount = 0;
            sprintf(q, "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '%s'  ORDER BY ORDINAL_POSITION", s.c_str());
            SQLExecDirect(hStmt, (SQLCHAR*)q, SQL_NTS);
            QStandardItemModel* EMP;
            int k = 0;
            cout << "B" << endl;
            SQLBindCol(hStmt, 1, SQL_C_CHAR, Name, sizeof(Name), NULL);
            while (SQLFetch(hStmt) != SQL_NO_DATA) {
                strcpy(colName[k], Name);
                cout << Name;
                k++;
            }
            SQLCloseCursor(hStmt);
            sprintf((char*)query, "SELECT * FROM %s", s.c_str());
            cout << query << endl;
            SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

            SQLNumResultCols(hStmt, &colCount);
            EMP = new QStandardItemModel(0,colCount);
            cout << colCount << endl;
            for (int i = 0; i < colCount; i++) {
                cout << i << endl;
                SQLBindCol(hStmt, i+1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName[i]).c_str()));
                
            }
            while (SQLFetch(hStmt) != SQL_NO_DATA) {
                string s;
                EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount+1).c_str()));
                cout << s << endl;
                for (int j = 0; j < colCount; j++) {
                    s = data[j];
                    if (nulldata[j] == SQL_NULL_DATA) {
                        EMP->setData(EMP->index(rowCount, j), "NULL");
                    }
                    else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));
                    
                }
                rowCount++;
            }
            ta->setModel(EMP);
            ui.tabWidget->addTab(ta, s.c_str());
            SQLCloseCursor(hStmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        }
        d->DBDisconnect();
    }
}
void DatabaseAssignment::server_select() {
    
    d = new Database();
    ui.combofor->clear();
    if (d->DBConnect() == true) {
        SQLHSTMT hstmt;
        if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hstmt) == SQL_SUCCESS) {
            int a;
            SQLExecDirect(hstmt, (SQLCHAR*)"select distinct SID_  from PLAYER", SQL_NTS);
            SQLNumResultCols(hstmt, (SQLSMALLINT*)&a);
            char number[50];
            SQLBindCol(hstmt, 1, SQL_C_CHAR, (SQLCHAR*)number, 50, NULL);
            while (SQLFetch(hstmt) != SQL_NO_DATA) {
                ui.combofor->addItem(QString(number));
            }
        }
        d->DBDisconnect();
    }
}
void DatabaseAssignment::date_select() {
    ui.combofor->clear();
    ui.combofor->addItem(toKor("월"));
    ui.combofor->addItem(toKor("화"));
    ui.combofor->addItem(toKor("수"));
    ui.combofor->addItem(toKor("목"));
    ui.combofor->addItem(toKor("금"));

}
void DatabaseAssignment::certain_select() {
    ui.combofor->clear();
    ui.combofor->addItem("ERROR_DETECT");
    ui.combofor->addItem("GAME");
    ui.combofor->addItem("SERVER_COMPUTER");
    ui.combofor->addItem("SERVER_MANAGER");
    ui.combofor->addItem("PLAYER");
    ui.combofor->addItem("QnA");
    ui.combofor->addItem("GAME_SERVER");
}
DatabaseAssignment::DatabaseAssignment(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    qDebug() << "A" << endl;
    int count = ui.tabWidget->count();
    for (int i = 0; i < count; i++) {
        ui.tabWidget->removeTab(0);
    }
    connect(ui.execute, SIGNAL(clicked(bool)), this, SLOT(excute()));
    connect(ui.Server_person, SIGNAL(clicked(bool)), this, SLOT(server_select()));
    connect(ui.date_check, SIGNAL(clicked(bool)), this, SLOT(date_select()));
    connect(ui.certain_value, SIGNAL(clicked(bool)), this, SLOT(certain_select()));
    initialize("ERROR_DETECT");
    initialize("GAME");
    initialize("SERVER_COMPUTER");
    initialize("SERVER_MANAGER");
    initialize("PLAYER");
    initialize("QnA");
    initialize("GAME_SERVER");
    ui.special_purpose->setVisible(false);
    ui.total->setChecked(true);
    ui.tabWidget->raise();
    
    
}
void DatabaseAssignment::excute() {
    if (ui.total->isChecked()) {
        int count = ui.tabWidget->count();
        for (int i = 0; i < count; i++) {
            ui.tabWidget->removeTab(0);
        }
        ui.special_purpose->setVisible(false);
        ui.tabWidget->setVisible(true);
        ui.tabWidget->raise();
        initialize("ERROR_DETECT");
        initialize("GAME");
        initialize("SERVER_COMPUTER");
        initialize("SERVER_MANAGER");
        initialize("PLAYER");
        initialize("QnA");
        initialize("GAME_SERVER");
    }
    else if (ui.less10m->isChecked()) {
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        d = new Database();
        cout << "A" << endl;
        QTableView* ta = new QTableView;
        if (d->DBConnect() == true) {
            static SQLCHAR query[250];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                
                SQLCloseCursor(hStmt);
                sprintf((char*)query, "SELECT SERVER_ID, GAME_ID, PEOPLE, START_TIME, COMID FROM GAME_SERVER E, GAME G WHERE E.GAME_ID = G.GAMEID AND DATEDIFF(MINUTE, E.START_TIME, CONVERT(TIME, GETDATE())) >= G.TIME OR DATEDIFF(MINUTE, E.START_TIME, CONVERT(TIME, GETDATE()))<0");//join 이용
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
                SQLNumResultCols(hStmt, &colCount);
                EMP = new QStandardItemModel(0, colCount);
                cout << colCount << endl;
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));
                        
                    }
                    rowCount++;
                }
                ui.special_purpose->setModel(EMP);
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }



    }
    else if (ui.QnA->isChecked()) {
        cout << "B" << endl;
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        d = new Database();
        QTableView* ta = new QTableView;
        if (d->DBConnect() == true) {
            static SQLCHAR query[100];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                
                sprintf((char*)query, "SELECT * FROM QnA WHERE CHECK_STATE = 0");
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

                SQLNumResultCols(hStmt, &colCount);
                EMP = new QStandardItemModel(0, colCount);
                cout << colCount << endl;
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));
                        
                    }
                    rowCount++;
                }
                ui.special_purpose->setModel(EMP);
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }
    }
    else if (ui.Server_person->isChecked()) {
        cout << "C" << endl;
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        if (d->DBConnect() == true) {
            static SQLCHAR query[100];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                cout << "B" << endl;
                
                SQLCloseCursor(hStmt);
                sprintf((char*)query, "SELECT * FROM PLAYER WHERE SID_ = '%s'", ui.combofor->currentText().toLocal8Bit().toStdString().c_str());
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
                SQLNumResultCols(hStmt, &colCount);
                EMP = new QStandardItemModel(0, colCount);
                cout << colCount << endl;
                cout << colCount << endl;
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));
                        
                    }
                    rowCount++;
                }
                ui.special_purpose->setModel(EMP);
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }

    }
    
    else {
        excute2();
    }
}
void DatabaseAssignment::excute2() {
    if (ui.date_check->isChecked()) {
        cout << "D" << endl;
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        QTableView* ta = new QTableView;
        if (d->DBConnect() == true) {
            static SQLCHAR query[150];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                SQLCloseCursor(hStmt);
                sprintf((char*)query, "SELECT SERVER_ID, GAME_ID, PEOPLE, START_TIME, E.COMID FROM GAME_SERVER E, SERVER_COMPUTER S WHERE E.COMID = S.COMID AND S.CHECK_DATE = '%s'", ui.combofor->currentText().toLocal8Bit().toStdString().c_str());//join 이용
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
                SQLNumResultCols(hStmt, &colCount);
                cout << colCount << endl;
                EMP = new QStandardItemModel(0, colCount);
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                cout << "result: " << colCount << endl;
                EMP = new QStandardItemModel(0, colCount);
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));

                    }
                    rowCount++;
                }
                cout << "asdf" << endl;
                if (rowCount == 0) {
                    QMessageBox msg;
                    msg.setText(toKor("해당 요일에는 점검할 서버가 없습니다!"));
                    msg.exec();
                }
                ui.special_purpose->setModel(EMP);
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }
    }
    else if (ui.morethan->isChecked()) {
        cout << "E" << endl;
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        QTableView* ta = new QTableView;
        if (d->DBConnect() == true) {
            static SQLCHAR query[100];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                SQLCloseCursor(hStmt);
                sprintf((char*)query, "SELECT * FROM GAME_SERVER WHERE PEOPLE>= '%s'", ui.get_number->toPlainText().toLocal8Bit().toStdString().c_str());
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
                SQLNumResultCols(hStmt, &colCount);
                cout << "result: " << colCount << endl;
                EMP = new QStandardItemModel(0, colCount);
                cout << colCount << endl;
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));

                    }
                    rowCount++;
                }
                if (rowCount == 0) {
                    QMessageBox msg;
                    msg.setText(toKor("해당하는 서버가 존재하지 않습니다!"));
                    msg.exec();
                }
                ui.special_purpose->setModel(EMP);
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }
    }
    else if (ui.certain_value->isChecked()) {
        ui.special_purpose->setVisible(true);
        ui.tabWidget->setVisible(false);
        ui.special_purpose->raise();
        d = new Database();
        if (d->DBConnect() == true) {
            static SQLCHAR query[100];
            SQLHSTMT hStmt;
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {

                char data[50][50];
                char colName[100];
                char Name[50];
                static char q[100];
                SQLLEN nulldata[50];
                SQLSMALLINT colCount = -1;
                int rowCount = 0;
                QStandardItemModel* EMP;
                int k = 0;
                cout << "B" << endl;
                sprintf((char*)query, "SELECT * FROM %s", ui.combofor->currentText().toLocal8Bit().toStdString().c_str());
                cout << query << endl;
                SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
                SQLNumResultCols(hStmt, &colCount);
                EMP = new QStandardItemModel(0, colCount);
                cout << colCount << endl;
                for (int i = 0; i < colCount; i++) {
                    SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)colName, 100, NULL, NULL, NULL, NULL, NULL);
                    cout << colName << endl;
                    EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName).c_str()));
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                }
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    string s;
                    bool t = false;
                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        cout << s << " " << ui.get_number->toPlainText().toLocal8Bit().toStdString().c_str() << endl;
                        if (s.find(ui.get_number->toPlainText().toLocal8Bit().toStdString().c_str()) != string::npos) {
                            cout << "true" << endl;
                            t = true;
                            break;
                        }
                    }
                    if (!t) {
                        continue;
                    }
                    EMP->setVerticalHeaderItem(rowCount, new QStandardItem(to_string(rowCount + 1).c_str()));
                    cout << s << endl;

                    for (int j = 0; j < colCount; j++) {
                        s = data[j];
                        if (nulldata[j] == SQL_NULL_DATA) {
                            EMP->setData(EMP->index(rowCount, j), "NULL");
                        }
                        else EMP->setData(EMP->index(rowCount, j), toKor(s.c_str()));

                    }
                    rowCount++;
                }
                ui.special_purpose->setModel(EMP);

                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            d->DBDisconnect();
        }

    }
}

