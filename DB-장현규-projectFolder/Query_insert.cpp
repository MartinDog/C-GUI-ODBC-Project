#include "Query_insert.h"
Query_insert::Query_insert(QWidget *parent)
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
    connect(ui.execute, SIGNAL(clicked(bool)), this, SLOT(exec_query()));
}

Query_insert::~Query_insert()
{
}
void Query_insert::exec_query() {
    Database* d = new Database(); 
    if (!d->DBConnect()) {
        return;
    }
    SQLHSTMT hstmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT,d->hDbc,&hstmt)==SQL_SUCCESS) {
        char query[180];
        if (ui.normal_input->isChecked()) {
            string tmp = ui.get_value->toPlainText().toLocal8Bit().toStdString().c_str();
            vector<string> v;
            int start = 0;
            int t = tmp.find(",");
            while (t != -1) {
                v.push_back(tmp.substr(start, t - start));
                start = t + 1;
                t = tmp.find(",", start);
            }
            v.push_back(tmp.substr(start, t - start));
            tmp = "";
            for (int i = 0; i < v.size(); i++) {
                tmp += "'" + v[i] + "'";
                if (i + 1 != v.size()) {
                    tmp += ',';
                }
            }
            cout <<"values: " <<tmp << endl;

            if (strcmp("PLAYER", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str()) == 0&&v.size()==3) {
                sprintf(query, "SELECT PING,PEOPLE,MAX_P FROM SERVER_COMPUTER S, GAME_SERVER G,GAME game WHERE G.SERVER_ID='%s' AND G.COMID = S.COMID AND game.GAMEID = G.GAME_ID", v[0].c_str());//join
                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    int ping;
                    int max_p;
                    int people;
                    SQLBindCol(hstmt, 1, SQL_C_LONG, &ping, 0, NULL);
                    SQLBindCol(hstmt, 2, SQL_C_LONG, &people, 0, NULL);
                    SQLBindCol(hstmt, 3, SQL_C_LONG, &max_p, 0, NULL);
                    SQLFetch(hstmt);
                    cout << ping << " " << people << " " << max_p << stoi(v[2].c_str())<< endl;
                    if (ping < stoi(v[2].c_str())) {
                        QMessageBox msg;
                        msg.setText(toKor("핑이 높아 추가할 수 없습니다."));
                        msg.exec();
                        SQLCloseCursor(hstmt);
                        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                        d->DBDisconnect();
                        return;
                    }
                    else if(people+1>max_p) {
                        QMessageBox msg;
                        msg.setText(toKor("최대 인원에 도달하여 인원을 추가할 수 없습니다."));
                        msg.exec();
                        SQLCloseCursor(hstmt);
                        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                        d->DBDisconnect();
                        return;
                    }
                    else {

                    }
                }
                else {
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    cout << buffer << endl;
                    QMessageBox msg;
                    msg.setText(QString((const char*)buffer));
                    msg.exec();
                }
            }
            SQLCloseCursor(hstmt);
            sprintf(query, "INSERT INTO %s VALUES(%s)", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str(),tmp.c_str());
            cout << query << endl;
            if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS)==SQL_SUCCESS) {
                if (strcmp("PLAYER", ui.comboBox->currentText().toLocal8Bit().toStdString().c_str()) == 0 && v.size() == 3) {
                    SQLCloseCursor(hstmt);
                    sprintf(query, "update GAME_SERVER set PEOPLE = TOTAL FROM(select SID_,COUNT(SID_) as TOTAL FROM PLAYER GROUP BY SID_) as t where t.SID_ = SERVER_ID");
                    cout << query << endl;
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                }
                QMessageBox msg;
                msg.setText(toKor("입력에 성공하였습니다!"));
                msg.exec();
            }
            else {
                SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                msg.setText(QString((const char*)buffer));
                msg.exec();
            }

        }
        else if (ui.QnAautoinput->isChecked()) {
            string tmp = ui.get_value->toPlainText().toLocal8Bit().toStdString().c_str();
            vector<string> v;
            int start = 0;
            int t = tmp.find(",");
            while (t != -1) {
                v.push_back(tmp.substr(start, t - start));
                start = t + 1;
                t = tmp.find(",", start);
            }
            v.push_back(tmp.substr(start, t - start));
            tmp = "";
            for (int i = 0; i < v.size(); i++) {
                tmp += "'" + v[i] + "'";
                if (i + 1 != v.size()) {
                    tmp += ',';
                }
            }
            cout << "values: " << tmp << endl;
            if (v.size() != 2) {
                QMessageBox msg;
                msg.setText(QString(toKor("값의 입력이 정확하지 않습니다. 다시 확인해 주십시오")));
                msg.exec();
            }
            else {
                int v1;
                vector<int> id;
                sprintf(query, "SELECT ID FROM SERVER_MANAGER except Select QMID FROM QnA");
                SQLBindCol(hstmt,1, SQL_C_LONG,(SQLSMALLINT*)&v1, 0, NULL);
                SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS);
                if (SQLFetch(hstmt) != SQL_NO_DATA) {

                    SQLCloseCursor(hstmt);
                    sprintf(query, "INSERT INTO QnA VALUES('%s','%s','%d',0)", v[0].c_str(), v[1].c_str(), v1);
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                    else {
                        QMessageBox msg;
                        msg.setText(toKor("성공! 관리자에게 답변을 할당 하였습니다."));
                        msg.exec();
                    }
                }
                else {
                    sprintf(query, "SELECT QMID FROM QnA GROUP BY QMID ORDER BY COUNT(QMID) ASC;");

                    SQLCloseCursor(hstmt);
                    SQLBindCol(hstmt, 1, SQL_C_LONG, (SQLSMALLINT*)&v1, 0, NULL);
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                    SQLFetch(hstmt);
                    SQLCloseCursor(hstmt);
                    sprintf(query, "INSERT INTO QnA VALUES('%s','%s','%d',0)", v[0].c_str(), v[1].c_str(), v1);
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                    else {
                        QMessageBox msg;
                        msg.setText(toKor("성공! 관리자에게 답변을 할당 하였습니다."));
                        msg.exec();
                    }

                }
            }
        }
        else if (ui.auto_player->isChecked()) {
            string tmp = ui.get_value->toPlainText().toLocal8Bit().toStdString().c_str();
            vector<string> v;
            int start = 0;
            int t = tmp.find(",");
            while (t != -1) {
                v.push_back(tmp.substr(start, t - start));
                start = t + 1;
                t = tmp.find(",", start);
            }
            v.push_back(tmp.substr(start, t - start));
            tmp = "";
            for (int i = 0; i < v.size(); i++) {
                tmp += "'" + v[i] + "'";
                if (i + 1 != v.size()) {
                    tmp += ',';
                }
            }
            cout << tmp << endl;
            if (v.size() == 3) {
                sprintf(query, "SELECT SERVER_ID,PING FROM SERVER_COMPUTER C,GAME_SERVER S,GAME G WHERE S.GAME_ID = '%s' AND S.GAME_ID = G.GAMEID AND G.MAX_P>S.PEOPLE AND S.COMID = C.COMID",v[1].c_str());
                char sid[100];
                int ping;
                cout << query << endl;
                SQLBindCol(hstmt, 1, SQL_C_CHAR, sid, 100, NULL);
                SQLBindCol(hstmt, 2, SQL_C_LONG, &ping, 0, NULL);
                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    cout << buffer << endl;
                    QMessageBox msg;
                    msg.setText(QString((const char*)buffer));
                    msg.exec();
                }
                else {
                    cout << sid << " " << ping;
                    bool t = true;
                    cout << "true" << endl;
                    while (SQLFetch(hstmt)!=SQL_NO_DATA) {
                        cout << "data" << endl;
                        if (ping >= stoi(v[2].c_str())) {
                            t = false;
                            SQLCloseCursor(hstmt);
                            sprintf(query, "INSERT INTO PLAYER VALUES('%s','%s','%s')",sid,v[0].c_str(),v[2].c_str());
                            if (SQLExecDirect(hstmt,(SQLCHAR*)query,SQL_NTS)!=SQL_SUCCESS) {
                                SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                SQLSMALLINT length;
                                SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                cout << buffer << endl;
                                QMessageBox msg;
                                msg.setText(QString((const char*)buffer));
                                msg.exec();
                            }
                            cout << "sucess" << endl;
                            break;
                        }
                    }
                    if (t) {
                        SQLCloseCursor(hstmt);
                        QMessageBox::StandardButton question;
                        char quest[100];
                        sprintf(quest, "현재 서버가 없어 인원을 추가할 수 없습니다. 서버를 추가하시겠습니까?");
                        string s(quest);
                        question = QMessageBox::question(this, toKor("서버 추가"), toKor(s.c_str()), QMessageBox::Yes | QMessageBox::No);
                        if (question == QMessageBox::Yes) {
                            vector<string> id;
                            char v1[100];
                            sprintf(query, "SELECT COMID FROM SERVER_COMPUTER WHERE PING >='%s' except Select COMID FROM GAME_SERVER",v[2].c_str());
                            cout << query << endl;
                            SQLBindCol(hstmt, 1, SQL_C_CHAR, v1,100, NULL);
                            SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS);
                            if (SQLFetch(hstmt) != SQL_NO_DATA) {
                                SQLCloseCursor(hstmt);
                                string s(v1);
                                sprintf(query, "INSERT INTO GAME_SERVER values((SELECT top(1) SERVER_ID FROM GAME_SERVER order by SERVER_ID desc)+1,'%s',1,CONVERT(TIME, GETDATE()),'%s')", v[1].c_str(), s.c_str());
                                cout << query << endl; 
                                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                    SQLSMALLINT length;
                                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                    cout << buffer << endl;
                                    QMessageBox msg;
                                    msg.setText(QString((const char*)buffer));
                                    msg.exec();
                                }
                                else {
                                    QMessageBox msg;
                                    msg.setText(toKor("성공! 서버를 추가하였습니다."));
                                    msg.exec();
                                    SQLCloseCursor(hstmt);
                                    sprintf(query, "INSERT INTO PLAYER VALUES((SELECT top(1) SERVER_ID FROM GAME_SERVER order by SERVER_ID desc),'%s','%s')", v[0].c_str(), v[2].c_str());
                                    cout << query << endl;
                                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                        SQLSMALLINT length;
                                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                        cout << buffer << endl;
                                        QMessageBox msg;
                                        msg.setText(QString((const char*)buffer));
                                        msg.exec();
                                    }
                                    else {
                                        QMessageBox msg;
                                        msg.setText(toKor("성공! 인원을 새로운 서버에 추가하였습니다."));
                                        msg.exec();
                                    }
                                }
                            }
                            else {
                                sprintf(query, "SELECT G.COMID FROM GAME_SERVER G,SERVER_COMPUTER C WHERE G.COMID = C.COMID AND C.PING >='%s' GROUP BY G.COMID ORDER BY COUNT(G.COMID) ASC;", v[2].c_str());
                                cout << query << endl;
                                SQLCloseCursor(hstmt);
                                SQLBindCol(hstmt, 1, SQL_C_CHAR, v1, 100, NULL);
                                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                    SQLSMALLINT length;
                                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                    cout << buffer << endl;
                                    QMessageBox msg;
                                    msg.setText(QString((const char*)buffer));
                                    msg.exec();
                                }
                                if (SQLFetch(hstmt)!=SQL_NO_DATA) {
                                    string s(v1);
                                    cout << s << endl;
                                    sprintf(query, "INSERT INTO GAME_SERVER VALUES((SELECT top(1) SERVER_ID FROM GAME_SERVER order by SERVER_ID desc)+1,'%s',1,CONVERT(TIME, GETDATE()),'%s')", v[1].c_str(), s.c_str());
                                    cout << "저기 " << v1 << " " << query << endl;
                                    SQLCloseCursor(hstmt);
                                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                        SQLSMALLINT length;
                                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                        cout << buffer << endl;
                                        QMessageBox msg;
                                        msg.setText(QString((const char*)buffer));
                                        msg.exec();
                                    }
                                    else {
                                        QMessageBox msg;
                                        msg.setText(toKor("성공! 서버를 추가하였습니다."));
                                        msg.exec();
                                        SQLCloseCursor(hstmt);
                                        sprintf(query, "INSERT INTO PLAYER VALUES((SELECT top(1) SERVER_ID FROM GAME_SERVER order by SERVER_ID desc),'%s','%s')", v[0].c_str(), v[2].c_str());
                                        cout << query << endl;
                                        if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                            SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                            SQLSMALLINT length;
                                            SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                            cout << buffer << endl;
                                            QMessageBox msg;
                                            msg.setText(QString((const char*)buffer));
                                            msg.exec();
                                        }
                                        else {
                                            QMessageBox msg;
                                            msg.setText(toKor("성공! 인원을 새로운 서버에 추가하였습니다."));
                                            msg.exec();
                                        }
                                    }
                                }
                                else {

                                    QMessageBox msg;
                                    msg.setText(toKor("서버 생성이 불가능 합니다!"));
                                    msg.exec();
                                }
                                
                            }
                            
                        }
                    }
                }
                SQLCloseCursor(hstmt);
                cout << "sync" << endl;
                sprintf(query, "update GAME_SERVER set PEOPLE = TOTAL FROM(select SID_,COUNT(SID_) as TOTAL FROM PLAYER GROUP BY SID_) as t where t.SID_ = SERVER_ID");
                cout << query << endl;
                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    cout << buffer << endl;
                    QMessageBox msg;
                    msg.setText(QString((const char*)buffer));
                    msg.exec();
                }
            }

        }
        else if (ui.auto_server->isChecked()) {
            string tmp = ui.get_value->toPlainText().toLocal8Bit().toStdString().c_str();
            vector<string> v;
            int start = 0;
            int t = tmp.find(",");
            while (t != -1) {
                v.push_back(tmp.substr(start, t - start));
                start = t + 1;
                t = tmp.find(",", start);
            }
            v.push_back(tmp.substr(start, t - start));
            tmp = "";
            for (int i = 0; i < v.size(); i++) {
                tmp += "'" + v[i] + "'";
                if (i + 1 != v.size()) {
                    tmp += ',';
                }
            }
            cout << "values: " << tmp << endl;
            if (v.size() != 4) {
                QMessageBox msg;
                msg.setText(QString(toKor("값의 입력이 정확하지 않습니다. 다시 확인해 주십시오")));
                msg.exec();
            }
            else {
                char v1[100];
                vector<int> id;
                sprintf(query, "SELECT ID FROM SERVER_MANAGER except Select MANAGER_ID FROM SERVER_COMPUTER");
                cout << "A" << query << endl;
                SQLBindCol(hstmt, 1, SQL_C_CHAR, v1, 100, NULL);
                SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS);
                if (SQLFetch(hstmt) != SQL_NO_DATA) {
                    cout << v1 << endl;
                    sprintf(query, "INSERT INTO SERVER_COMPUTER VALUES('%s','%s','%s','%s','%s')", v[0].c_str(), v[1].c_str(), v[2].c_str(), v[3].c_str(), v1);
                    cout << "B" << query << endl;
                    SQLCloseCursor(hstmt);
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                    else {
                        QMessageBox msg;
                        msg.setText(toKor("성공! 서버 추가및 관리자 할당에 성공하였습니다."));
                        msg.exec();
                    }
                }
                else {
                    sprintf(query, "SELECT MANAGER_ID FROM SERVER_COMPUTER GROUP BY MANAGER_ID ORDER BY COUNT(MANAGER_ID) ASC;");
                    cout << "C" << query << endl;
                    SQLCloseCursor(hstmt);
                    SQLBindCol(hstmt, 1, SQL_C_CHAR, v1, 100, NULL);
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                    if (SQLFetch(hstmt) != SQL_NO_DATA) {
                        sprintf(query, "INSERT INTO SERVER_COMPUTER VALUES('%s','%s','%s','%s','%s')", v[0].c_str(), v[1].c_str(), v[2].c_str(), v[3].c_str(), v1);
                        cout << "D" << query << endl;
                        SQLCloseCursor(hstmt);
                        if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                            SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                            SQLSMALLINT length;
                            SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                            cout << buffer << endl;
                            QMessageBox msg;
                            msg.setText(QString((const char*)buffer));
                            msg.exec();
                        }
                        else {
                            QMessageBox msg;
                            msg.setText(toKor("성공! 서버 추가및 관리자 할당에 성공하였습니다."));
                            msg.exec();
                        }
                    }
                    else {
                        QMessageBox msg;
                        msg.setText(toKor("서버 컴퓨터를 추가할 수 없습니다."));
                        msg.exec();
                    }

                }
            }
        }
        else if (ui.auto_ERROR_DETECT->isChecked()) {
            sprintf(query, "SELECT COMID FROM SERVER_COMPUTER EXCEPT SELECT SUPERVISED FROM ERROR_DETECT");
            if (SQLExecDirect(hstmt,(SQLCHAR*) query, SQL_NTS)==SQL_SUCCESS) {
                char comid[100];
                vector<string> v;
                vector<string> supervise;
                SQLBindCol(hstmt, 1, SQL_C_CHAR, comid, 100, NULL);
                while (SQLFetch(hstmt)!=SQL_NO_DATA) {
                    v.push_back(string(comid));
                    cout << comid << endl;
                }
                if (v.size() == 0) {
                    QMessageBox msg;
                    msg.setText(toKor("모든 서버가 감시받고 있습니다!"));
                    msg.exec();
                }
                else {
                    SQLCloseCursor(hstmt);
                    sprintf(query, "SELECT COMID FROM SERVER_COMPUTER EXCEPT SELECT SUPERVISE FROM ERROR_DETECT");
                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                        cout << "A" << endl;
                        SQLBindCol(hstmt, 1, SQL_C_CHAR, comid, 100, NULL);
                        while (SQLFetch(hstmt) != SQL_NO_DATA) {
                            supervise.push_back(string(comid));
                            cout << comid << endl;
                        }
                        if (supervise.size()>1) {
                            int j = 0;
                            for (int i = 0; i < v.size(); i++) {
                                SQLCloseCursor(hstmt);
                                cout << "B" << endl;
                                if (supervise[j % supervise.size()].find(v[i]) != string::npos) {
                                    j+=2;
                                    continue;
                                }
                                sprintf(query, "INSERT INTO ERROR_DETECT VALUES('%s','%s')",supervise[j%supervise.size()].c_str() ,v[i].c_str());
                                cout << query << endl;
                                j++;
                                if (SQLExecDirect(hstmt,(SQLCHAR*)query,SQL_NTS)!=SQL_SUCCESS) {
                                    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                    SQLSMALLINT length;
                                    SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                    cout << buffer << endl;
                                    QMessageBox msg;
                                    msg.setText(QString((const char*)buffer));
                                    msg.exec();
                                }
                                else {
                                    cout << "B1" << endl;
                                    string total = "컴퓨터 " + supervise[j % supervise.size()] + "가" + v[i] + "를 감시합니다";
                                    QMessageBox msg;
                                    msg.setText(toKor(total.c_str()));
                                    msg.show();
                                }
                            }
                        }
                        else {
                            cout << "C" << endl;
                            for (int i = 0; i < v.size();) {
                                SQLCloseCursor(hstmt);
                                sprintf(query, "SELECT SUPERVISE FROM ERROR_DETECT GROUP BY SUPERVISE ORDER BY COUNT(SUPERVISE) ASC");
                                if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                                    SQLBindCol(hstmt, 1, SQL_C_CHAR, comid, 100, NULL);
                                    while (SQLFetch(hstmt) != SQL_NO_DATA) {
                                        if (v[i].find(string(comid)) != string::npos) {
                                            continue;
                                        }
                                        else {
                                            break;
                                        }
                                    }
                                    
                                    sprintf(query, "INSERT INTO ERROR_DETECT VALUES('%s','%s')", comid, v[i].c_str());
                                    
                                    i++;
                                    cout << query << endl;
                                    SQLCloseCursor(hstmt);
                                    if (SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                                        SQLSMALLINT length;
                                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                                        cout << buffer << endl;
                                        QMessageBox msg;
                                        msg.setText(QString((const char*)buffer));
                                        msg.exec();
                                    }
                                    else {
                                        cout << "A1" << endl;
                                        QMessageBox msg;
                                        msg.setText(toKor("할당에 성공하였습니다!"));
                                        msg.show();
                                    }
                                }
                            }
                        }
                    }
                    else {
                        cout << "DD" << endl;
                        SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hstmt, NULL, NULL, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        msg.setText(QString((const char*)buffer));
                        msg.exec();
                    }
                }
            }
        }
        else {

        }
    }
    else {
        
    }
    SQLCloseCursor(hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    d->DBDisconnect();
}
