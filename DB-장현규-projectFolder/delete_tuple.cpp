#include "delete_tuple.h"
#include "DatabaseAssignment.h"
#include<iostream>
#include<string>
#include<vector>
using namespace std;
string** key;
int lengths[7] = {2,1,1,1,2,3,1};
void initializer() {
    key = new string * [7];
    key[0] = new string[2]{ {"SUPERVISE"}, {"SUPERVISED"} };
    key[1] = new  string[]{ {"GAMEID"} };
    key[2] = new string[]{ {"COMID"} };
    key[3] = new string[]{ {"ID"} };
    key[4] = new string[2]{ {"SID_"}, {"PID"} };
    key[5] = new string[]{ {"QSID"},{"QPID"},{"QMID"} };
    key[6] = new string[]{ {"SERVER_ID"} };

}
delete_tuple::delete_tuple(QWidget* parent)
	: QWidget(parent)
{
	uis.setupUi(this);
    uis.combofor->addItem("ERROR_DETECT");
    uis.combofor->addItem("GAME");
    uis.combofor->addItem("SERVER_COMPUTER");
    uis.combofor->addItem("SERVER_MANAGER");
    uis.combofor->addItem("PLAYER");
    uis.combofor->addItem("QnA");
    uis.combofor->addItem("GAME_SERVER");
    connect(uis.excute, SIGNAL(clicked(bool)), this, SLOT(execute()));
    initializer();

}

delete_tuple::~delete_tuple()
{
}
void delete_tuple::execute() {
    Database* d = new Database();
    if (!d->DBConnect()) {
        return;
    }
    static SQLCHAR query[150];
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, d->hDbc, &hStmt) == SQL_SUCCESS) {
        if (uis.Keydelete->isChecked()) {
            string keysum = " ";
            string inform = uis.get_key->toPlainText().toLocal8Bit().toStdString();
            vector<string> v = vector<string>();
            int start = 0;
            int t = inform.find(",");
            while (t != -1) {
                v.push_back(inform.substr(start, t - start));
                start = t + 1;
                t = inform.find(",", start);
            }
            v.push_back(inform.substr(start, t - start));
            if (v.size() != lengths[uis.combofor->currentIndex()]) {
                QMessageBox msg;
                msg.setText(toKor("키값의 수가 일치하지 않습니다. 다시 입력 해주세요"));
                msg.exec();
                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                d->DBDisconnect();
                return;
            }
            cout << v.size() << endl;;
            for (int i = 0; i < v.size(); i++) {
                cout << key[uis.combofor->currentIndex()][i] << endl;
                keysum = keysum + key[uis.combofor->currentIndex()][i] + "='" + v[i]+"'";
                if (i != v.size() - 1) {
                    keysum += " AND ";
                }
            }
            sprintf((char*)query, "DELETE FROM %s WHERE %s", uis.combofor->currentText().toLocal8Bit().toStdString().c_str(), keysum.c_str());
            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS)==SQL_SUCCESS) {
                if (strcmp(uis.combofor->currentText().toLocal8Bit().toStdString().c_str(), "PLAYER") == 0) {
                    SQLCloseCursor(hStmt);
                    sprintf((char *)query, "update GAME_SERVER set PEOPLE = TOTAL FROM(select SID_, COUNT(SID_) as TOTAL FROM PLAYER GROUP BY SID_) as t where t.SID_ = SERVER_ID");
                    if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) != SQL_SUCCESS) {
                        char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox errmsg;
                        QString s(toKor(buffer));
                        errmsg.setText(s);
                        errmsg.exec();
                    }
                }
                QMessageBox msg;
                msg.setText(toKor("성공"));
                msg.exec();
            }
            else {
                char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                QString s(toKor(buffer));
                msg.setText(s);
                msg.exec();
            }
        }
        else if (uis.QnA_del->isChecked()) {
            sprintf((char*)query, "DELETE FROM QnA WHERE CHECK_STATE = 1");
            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                cout << "test" << endl;
                QMessageBox msg;
                msg.setText(toKor("성공"));
                msg.exec();
            }
            else {
                cout << "test1" << endl;
                char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                msg.setText(toKor(buffer));
                if (strlen(buffer) != 0) {
                    msg.exec();
                }
            }
        }
        else if (uis.server_computer->isChecked()) {
            string comid = uis.get_key->toPlainText().toLocal8Bit().toStdString().c_str();
            sprintf((char*)query,"DELETE FROM ERROR_DETECT WHERE SUPERVISE = '%s'",comid.c_str() );
            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                QMessageBox msg;

                SQLCloseCursor(hStmt);
                sprintf((char*)query, "DELETE FROM SERVER_COMPUTER WHERE COMID = '%s'", comid.c_str());
                if (SQLExecDirect(hStmt,(SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    QMessageBox msg;
                    cout << "test2" << endl;
                    msg.setText(toKor("성공"));
                    msg.exec();
                }
                else {
                    cout << "test3" << endl;
                    char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    cout << buffer << endl;
                    QMessageBox msg;
                    QString s(toKor(buffer));
                    msg.setText(s);
                    if (strlen(buffer) != 0) {
                        msg.exec();
                    }
                }
            }
            else {
                char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                QString s(toKor(buffer));
                msg.setText(s); 
                if (strlen(buffer) != 0) {
                    msg.exec();
                }
            }
            
        }
        else if (uis.server_manager->isChecked()) {
            string id = uis.get_key->toPlainText().toLocal8Bit().toStdString().c_str();
            sprintf((char*)query, "SELECT * FROM QnA WHERE QMID = '%s' AND CHECK_STATE = 0", id.c_str());
            cout << query << endl;
            char Qmid[100], Qsid[100], Qpid[100];
            if (SQLExecDirect(hStmt,(SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                QMessageBox msg;
                SQLBindCol(hStmt, 1, SQL_C_CHAR, (SQLCHAR*)Qmid, 100, NULL);
                SQLBindCol(hStmt, 2, SQL_C_CHAR, (SQLCHAR*)Qsid, 100, NULL);
                SQLBindCol(hStmt, 3, SQL_C_CHAR, (SQLCHAR*)Qpid, 100, NULL);
                vector<pair<string, pair<string, string>>> v;
                while (SQLFetch(hStmt) != SQL_NO_DATA) {
                    QMessageBox::StandardButton question;
                    char quest[100];
                    int i;
                    for (i = 0; Qsid[i] != '\0' && Qsid[i] != ' '; i++) {
                    }
                    Qsid[i+1] = '\0';
                    sprintf(quest, "현재 %s 매니저는 %s서버 %s플레이어의 답변을 처리 하지 않았습니다. 처리 하셨습니까?",  Qpid, Qmid, Qsid);
                    string s(quest);
                    question = QMessageBox::question(this, toKor("매니저 삭제"), toKor(s.c_str()), QMessageBox::Yes | QMessageBox::No);
                    if (question == QMessageBox::Yes) {
                        v.push_back(pair<string,pair<string,string>>(string(Qmid),pair<string,string>(string(Qpid),string(Qsid))));
                        cout << v[0].first.c_str() << " " << v[0].second.first.c_str() << " " << v[0].second.second.c_str() << endl;
                    }
                    else {
                        continue;
                    }
                }
                SQLCloseCursor(hStmt);
                for (vector<pair<string, pair<string, string>>>::iterator iter=v.begin(); iter != v.end(); iter++) {
                    sprintf((char*)query, "DELETE FROM QnA WHERE QSID = '%s' AND QMID = '%s' AND QPID = '%s'", iter->first.c_str(),iter->second.first.c_str(),iter->second.second.c_str());
                    cout << query << endl;
                    SQLCloseCursor(hStmt);
                    if (SQLExecDirect(hStmt,(SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                        QMessageBox msg;
                        msg.setText(toKor("삭제에 성공하였습니다."));
                        msg.exec();
                    }
                    else {
                        char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                        SQLSMALLINT length;
                        SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                        cout << buffer << endl;
                        QMessageBox msg;
                        QString s(toKor(buffer));
                        msg.setText(s);
                        msg.exec();
                    }
                }
                SQLCloseCursor(hStmt);
                sprintf((char*)query, "DELETE FROM SERVER_MANAGER WHERE ID = '%s'", id.c_str());
                SQLCloseCursor(hStmt);
                if (SQLExecDirect(hStmt,(SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
                    QMessageBox msg;
                    msg.setText(toKor("서버 매니저 삭제에 삭제에 성공하였습니다."));
                }
                else {
                    char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                    SQLSMALLINT length;
                    SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                    if(strlen(buffer)==0)
                        sprintf(buffer, "해당하는 인원이 존재하지 않습니다.");
                    QMessageBox msg;
                    QString s(toKor(buffer));
                    msg.setText(s);
                    msg.exec();

                }
            }
            else {
                char buffer[SQL_MAX_MESSAGE_LENGTH + 1];
                SQLSMALLINT length;
                SQLError(d->hEnv, d->hDbc, hStmt, NULL, NULL, (SQLCHAR*)buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
                cout << buffer << endl;
                QMessageBox msg;
                QString s(toKor(buffer));
                msg.setText(s);
                msg.exec();
            }
        }
        else {

        }
        SQLCloseCursor(hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
    d->DBDisconnect();
}