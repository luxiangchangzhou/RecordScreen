#include "RecordScreen.h"
#include <QtWidgets/QApplication>
#include "LX_AVRecord.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RecordScreen w;
	w.show();

	/*LX_AVRecord avRecord;
	avRecord.init("123.mp4", true);
	avRecord.startRecord();
	cin.get();
	avRecord.stop();
	avRecord.clear();

	avRecord.init("321.mp4", false, 0);
	avRecord.startRecord();
	cin.get();
	avRecord.stop();
	avRecord.clear();*/




	return a.exec();
}
