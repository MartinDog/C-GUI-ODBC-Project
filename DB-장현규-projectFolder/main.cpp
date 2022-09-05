#include "DatabaseAssignment.h"
#include "delete_tuple.h"
#include "Main_Menu.h"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
    Database* d = new Database();
    d->DBConnect();
    d->DBDisconnect();
    QApplication a(argc, argv);
    Main_Menu w;
    w.show();
    return a.exec();
}
