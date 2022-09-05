#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<stdio.h>
#include<Windows.h>
#include<sql.h>
#include<sqlext.h>
#include <QtWidgets/QWidget>
#include<qdebug.h>
#include<qstandarditemmodel.h>
#include "ui_DatabaseAssignment.h"
#include<string>
#include <QMessageBox>
#include <qtableview.h>
#include<qstring.h>
#define toKor(str) QString::fromLocal8Bit(str)
using namespace std;
class Database;
class Database {
public:
    bool DBConnect();
    void DBDisconnect();
    SQLHENV hEnv = NULL;//�ڵ鰪 ����
    SQLHDBC hDbc = NULL;//���� ������ ���� �ڵ鰪 ���� hDbc�� hEnv�� �ʿ���


};
class DatabaseAssignment : public QWidget
{
    Q_OBJECT

public:
    DatabaseAssignment(QWidget *parent = Q_NULLPTR);
    Ui::DatabaseAssignmentClass ui;
    Database *d=NULL;
    QTableView* table = new QTableView;
    void initialize(string s);
private:
    string attribute[2];
    
public slots:
    void excute();
    void excute2();
    void server_select();
    void date_select();
    void certain_select();
};
