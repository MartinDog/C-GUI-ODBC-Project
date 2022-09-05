#include "update_query.h"
#include "DatabaseAssignment.h"
update_query::update_query(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    ui.comboBox->addItem("ERROR_DETECT");
    ui.comboBox->addItem("GAME");
    ui.comboBox->addItem("SERVER_COMPUTER");
    ui.comboBox->addItem("SERVER_MANAGER");
    ui.comboBox->addItem("PLAYER");
    ui.comboBox->addItem("QnA");
    ui.comboBox->addItem("GAME_SERVER");
    connect(ui.comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(change_value(QString)));
    connect(ui.exec, SIGNAL(clicked(bool)), this, SLOT(execution()));
    change_value(QString("ERROR_DETECT"));
}
update_query::~update_query()
{
}
void update_query::change_value(QString e) {
    std::cout << e.toLocal8Bit().toStdString().c_str() << endl;
    Database* d = NULL;
    d = new Database();
    string s = e.toLocal8Bit().toStdString().c_str();
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
            EMP = new QStandardItemModel(0, colCount);
            cout << colCount << endl;
            for (int i = 0; i < colCount; i++) {
                cout << i << endl;
                SQLBindCol(hStmt, i + 1, SQL_C_CHAR, data[i], sizeof(data[i]), &nulldata[i]);
                EMP->setHeaderData(i, Qt::Horizontal, QObject::tr(string(colName[i]).c_str()));

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
            ui.tableView->setModel(EMP);
            SQLCloseCursor(hStmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        }
        d->DBDisconnect();
    }
}
void update_query::execution() {
    int lengths[7] = { 2,1,1,1,2,3,1 };
    Database* d = NULL;
    d = new Database();
    if (!d->DBConnect()) {
        return;
    }
    static SQLCHAR query[150];
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
        if (ui.normal->isChecked()) {
            char query[100];
            char** tmp;
            char** key;
            int rowCount;
            int colCount;
            vector<string> tkey = vector<string>();
            if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
                rowCount = (ui.tableView)->model()->rowCount();
                colCount = (ui.tableView)->model()->columnCount();
                tmp = new char* [colCount];
                key = new char* [colCount];
                for (int i = 0; i < colCount; i++) {
                    tmp[i] = new char[100];
                    key[i] = new char[100];
                }
                SQLLEN nulldata[50];
                cout << rowCount << " " << colCount << endl;
                sprintf(query, "SELECT * FROM %s", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str());
                if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    for (int j = 0; j < rowCount; j++) {
                        for (int i = 0; i < lengths[ui.comboBox->currentIndex()]; i++) {
                            SQLDescribeCol(hStmt, i + 1, (SQLCHAR*)key[i], 100, NULL, NULL, NULL, NULL, NULL);
                            SQLBindCol(hStmt, i + 1, SQL_C_CHAR, (SQLCHAR*)tmp[i], 100, nulldata + i);
                        }
                        if (SQLFetch(hStmt) != SQL_NO_DATA) {
                            for (int i = 0; i < lengths[ui.comboBox->currentIndex()]; i++) {
                                string keys(key[i]);
                                string values(tmp[i]);
                                tkey.push_back(keys + " = " + "'" + values + "'");
                            }
                        }
                    }
                }
                else {
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL,(SQLCHAR*) buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    cout << buffer << endl;
                    QMessageBox msg;
                    msg.setText(toKor(string((char*)buffer).c_str()));
                    msg.exec();
                    SQLCloseCursor(hStmt);
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                    d->DBDisconnect();
                    return;
                }

                SQLCloseCursor(hStmt);
                int count = 0;
                for (int j = 0; j < rowCount;) {
                    string keys = "";
                    for (int i = 0; i < lengths[ui.comboBox->currentIndex()]; i++) {
                        keys = keys + tkey[count++];
                        if (i + 1 != lengths[ui.comboBox->currentIndex()]) {
                            keys += " AND ";
                        }
                    }
                    cout << "key " << keys << endl;
                    for (int k = 0; k < colCount; k++) {
                        string* ns = NULL;
                        if (strcmp((ui.tableView)->model()->data((ui.tableView)->model()->index(j, k)).toString().toLocal8Bit().toStdString().c_str(), "NULL") == 0) {
                            ns = new string("null");
                        }
                        else {
                            ns = new string(((ui.tableView)->model()->data(((ui.tableView)->model()->index(j, k))).toString().toLocal8Bit().toStdString()));
                        }
                        string st = ((ui.tableView)->model()->headerData(k, Qt::Horizontal).toString().toLocal8Bit());
                        string tmpkey = keys.substr(keys.find("= ") + 2, keys.find(" A") - 6);
                        cout << "subkeys: " << tmpkey << endl;
                        if (strcmp("PLAYER", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str()) == 0 && strcmp("SID_", st.c_str()) == 0 && tmpkey.find((*ns)) == -1) {
                            int pingping;
                            SQLCloseCursor(hStmt);
                            cout << "COMPARE" << tmpkey << " " << *ns << endl;
                            if (strcmp((*ns).c_str(), "null") != 0) {
                                *ns = "'" + *ns + "'";
                            }
                            sprintf(query, "SELECT * FROM  GAME_SERVER G, GAME GG WHERE  G.SERVER_ID= %s AND GG.GAMEID = G.GAME_ID AND G.PEOPLE+1<=GG.MAX_P", (*ns).c_str());
                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                                if (SQLFetch(hStmt) == SQL_NO_DATA) {
                                    QMessageBox msg;
                                    msg.setText(toKor("서버가 가득차 인원을 추가할 수 없습니다."));
                                    msg.exec();
                                    cout << "k:" << k << " " << keys << endl;
                                    break;
                                }
                            }
                        }
                        else if (strcmp("PLAYER", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str()) == 0 && strcmp("PING", st.c_str()) == 0) {
                            int pingping;
                            SQLCloseCursor(hStmt);
                            sprintf(query, "SELECT C.PING FROM PLAYER P, GAME_SERVER G,SERVER_COMPUTER C WHERE %s AND P.SID_ = G.SERVER_ID AND C.COMID = G.COMID", keys.c_str());
                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                                SQLBindCol(hStmt, 1, SQL_C_LONG, &pingping, 0, NULL);
                                if (SQLFetch(hStmt) != SQL_NO_DATA) {
                                    if (pingping < stoi(*ns)) {
                                        QMessageBox::StandardButton question;
                                        char quest[100];
                                        sprintf(quest, "새로 변경된 핑이 제한 핑을 초과하였습니다. 해당 플레이어를 추방하시겠습니까?");
                                        string s(quest);
                                        question = QMessageBox::question(this, toKor("서버 추가"), toKor(s.c_str()), QMessageBox::Yes | QMessageBox::No);
                                        if (question == QMessageBox::Yes) {
                                            SQLCloseCursor(hStmt);
                                            sprintf(query, "delete from PLAYER where %s", keys.c_str());
                                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                                                QMessageBox msg;
                                                cout << "player boomboom" << endl;
                                                msg.setText(toKor("성공적으로 추방하였습니다!"));
                                                sprintf(query, "update GAME_SERVER set PEOPLE = TOTAL FROM(select SID_,COUNT(SID_) as TOTAL FROM PLAYER GROUP BY SID_) as t where t.SID_ = SERVER_ID");
                                                cout << query << endl;
                                                SQLCloseCursor(hStmt);
                                                if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                                    SQLSMALLINT length;
                                                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                                    cout << buffer << endl;
                                                    QMessageBox msg;
                                                    msg.setText(toKor(string((char*)buffer).c_str()));
                                                    msg.exec();
                                                }
                                                msg.exec();
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (strcmp((*ns).c_str(), "null") != 0) {
                            *ns = "'" + *ns + "'";
                        }
                        sprintf((char*)query, "UPDATE %s set %s where %s", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str(), (st + "=" +  *ns ).c_str(), keys.c_str());
                        cout << query << endl;
                        SQLCloseCursor(hStmt);
                        if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_ERROR) {
                            SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                            SQLSMALLINT length;
                            SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL,(SQLCHAR*) buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                            cout << buffer << endl;
                            QMessageBox msg;
                            msg.setText(toKor(string((char*)buffer).c_str()));
                            msg.exec();
                        }
                        if (strcmp("PLAYER", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str()) == 0) {
                            sprintf(query, "update GAME_SERVER set PEOPLE = TOTAL FROM(select SID_,COUNT(SID_) as TOTAL FROM PLAYER GROUP BY SID_) as t where t.SID_ = SERVER_ID");
                            cout << query << endl;
                            SQLCloseCursor(hStmt);
                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                SQLSMALLINT length;
                                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                cout << buffer << endl;
                                QMessageBox msg;
                                msg.setText(toKor(string((char*)buffer).c_str()));
                                msg.exec();
                            }
                        }
                    }
                    j++;
                }

            }

        }
        else if (ui.server_ma->isChecked()) {
            sprintf((char*)query, "SELECT COMID FROM SERVER_COMPUTER WHERE MANAGER_ID is NULL;");
            char comid[100];
            vector<string> vnull;
            cout << query << endl;
            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS)==SQL_SUCCESS) {
                SQLBindCol(hStmt, 1, SQL_C_CHAR, comid, 100, NULL);
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    vnull.push_back(string(comid));
                }
                if (vnull.size() == 0) {
                    QMessageBox msg;
                    msg.setText(toKor("모든 컴퓨터에 관리자가 할당되어 있습니다."));
                    msg.exec();
                    
                }
                else {
                    vector<string> manager;
                    sprintf((char*)query, "SELECT ID FROM SERVER_MANAGER EXCEPT SELECT MANAGER_ID FROM SERVER_COMPUTER");
                    SQLCloseCursor(hStmt);
                    cout << query << endl;

                    if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                        SQLBindCol(hStmt, 1, SQL_C_CHAR, comid, 100, NULL);
                        while (SQLFetch(hStmt) != SQL_NO_DATA) {
                            manager.push_back(string(comid));
                        }
                        if (manager.size() == 0) {
                            SQLCloseCursor(hStmt);
                            sprintf((char*)query, "SELECT MANAGER_ID FROM SERVER_COMPUTER GROUP BY MANAGER_ID ORDER BY COUNT(MANAGER_ID)ASC");
                            cout << query << endl;
                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                                SQLBindCol(hStmt, 1, SQL_C_CHAR, comid, 100, NULL);
                                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                                    manager.push_back(string(comid));
                                }
                            }
                        }
                        SQLCloseCursor(hStmt);
                        for (int i = 0; i < vnull.size(); i++) {
                            sprintf((char*)query, "UPDATE SERVER_COMPUTER SET MANAGER_ID = '%s' WHERE COMID='%s'", manager[i % manager.size()].c_str(), vnull[i].c_str());

                            cout << query << endl; 
                            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS)==SQL_SUCCESS) {
                                string buffer = vnull[i].substr(0,vnull[i].find(" ")) +"번 컴퓨터를"+ manager[i % manager.size()]+"번 관리자에게 할당하였습니다.";
                                QMessageBox msg;
                                msg.setText(toKor(buffer.c_str()));
                                msg.exec();
                            }
                        }
                    }
                    else {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(toKor(string((char*)buffer).c_str()));
                        msg.exec();
                    }
                }
            }
            else {
                SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                msg.setText(toKor(string((char*)buffer).c_str()));
                msg.exec();
            }
        }
        else if (ui.supervise->isChecked()) {
            string tmp = ui.plainTextEdit->toPlainText().toLocal8Bit().toStdString().c_str();
            vector<string> v;
            int start = 0;
            int t = tmp.find(",");
            while (t != -1) {
                v.push_back(tmp.substr(start, t - start));
                start = t + 1;
                t = tmp.find(",", start);
            }
            v.push_back(tmp.substr(start, t - start));
            if (v.size() == 2) {
                vector<string> sid;
                char comid[100];
                sprintf((char*)query, "SELECT COMID FROM SERVER_COMPUTER WHERE MANAGER_ID = '%s'",v[0].c_str() );
                if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    SQLBindCol(hStmt, 1, SQL_C_CHAR, comid, 100, NULL);
                    while (SQLFetch(hStmt)!=SQL_NO_DATA) {
                        sid.push_back(string(comid));
                    }
                    bool t=true;
                    for (int i = 0; i < sid.size(); i++) {
                        SQLCloseCursor(hStmt);
                        sprintf((char*)query, "UPDATE SERVER_COMPUTER SET CHECK_DATE = '%s' WHERE COMID = '%s'",v[1].c_str(),sid[i].c_str());
                        if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                            t = false;
                            SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                            SQLSMALLINT length;
                            SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                            QMessageBox msg;
                            cout << buffer << endl;
                            msg.setText(toKor(string((char*)buffer).c_str()));
                            msg.exec();
                        }
                    }
                    if(t) {
                        QMessageBox msg;
                        msg.setText(toKor("해당되는 모든 서버의 점검 요일을 변경하였습니다!"));
                        msg.exec();
                    }
                }
                else {
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);

                    QMessageBox msg;
                    cout << buffer << endl;
                    msg.setText(toKor(string((char*)buffer).c_str()));
                    msg.exec();
                }
            }
            else {
                QMessageBox msg;
                msg.setText(toKor("값을 다시 확인해 주세요."));
                msg.exec();
            }
        }
        else if (ui.updatetme->isChecked()) {
            char sid[100];
            sprintf((char*)query, "SELECT SERVER_ID FROM GAME_SERVER E, GAME G WHERE E.GAME_ID = G.GAMEID AND ABS(DATEDIFF(MINUTE, E.START_TIME, CONVERT(TIME, GETDATE()))) >= G.TIME");
            cout << query << endl;
            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS)==SQL_SUCCESS) {
                vector<string> v;
                SQLBindCol(hStmt,1,SQL_C_CHAR,(SQLCHAR*)sid,100,NULL);
                SQLCloseCursor(hStmt);
                if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    while (SQLFetch(hStmt) != SQL_NO_DATA) {
                        v.push_back(string(sid));
                        cout << sid << endl;
                    }
                    SQLCloseCursor(hStmt);
                    for (int i = 0; i < v.size(); i++) {
                        sprintf((char*)query, "UPDATE GAME_SERVER SET START_TIME = CONVERT(TIME,GETDATE()) WHERE SERVER_ID = '%s'", v[i].c_str());
                        cout << query << endl;
                        if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                            QMessageBox msg;
                            string t = v[i] + "서버의 시간이 갱신되었습니다!";
                            msg.setText(toKor(t.c_str()));
                            msg.exec();
                        }
                        else {
                            SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                            SQLSMALLINT length;
                            SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                            cout << buffer << endl;
                            QMessageBox msg;
                            msg.setText(toKor(string((char*)buffer).c_str()));
                            msg.exec();
                        }
                    }
                }
                else {
                    cout << "B" << endl;
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    QMessageBox msg;
                    cout << buffer << endl;
                    msg.setText(toKor(string((char*)buffer).c_str()));
                    msg.exec();
                }
                
            }
            else {
                cout << "A" << endl;
                SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                
                QMessageBox msg;
                cout << buffer << endl;
                msg.setText(toKor(string((char*)buffer).c_str()));
                msg.exec();
            }


        }
        else {
        cout << "ERROR" << endl;
        }
    }
    SQLCloseCursor(hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    d->DBDisconnect();
}