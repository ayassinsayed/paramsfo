#include <QtCore/QCoreApplication>
#include <QtCore/qdebug.h>
#include "PARAM.h"


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	PARAM("c:\\x.sfo").remove("app_ver");
	qDebug() << "finish";
	getchar();
	//return a.exec();
}