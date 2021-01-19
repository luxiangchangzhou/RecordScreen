#include "RecordScreen.h"
#include <iostream>
#include <QFileDialog>
#include <QTime>
#include <QDir>


using namespace std;

static QTime qtime;

char*start_style =
"QPushButton:!hover\
{\
	border-image: url(:/RecordScreen/Resources/start.png);\
}\
QPushButton:hover\
{\
	border-image: url(:/RecordScreen/Resources/start_hover.png);\
}\
QPushButton:pressed\
{\
	border-image: url(:/RecordScreen/Resources/start_pressed.png);\
}";

char*stop_style =
"QPushButton:!hover\
{\
	border-image: url(:/RecordScreen/Resources/stop.png);\
}\
QPushButton:hover\
{\
	border-image: url(:/RecordScreen/Resources/stop_hover.png);\
}\
QPushButton:pressed\
{\
	border-image: url(:/RecordScreen/Resources/stop_pressed.png);\
}";

RecordScreen::RecordScreen(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	QWidget::startTimer(300);
	ui.lineEdit_dir->setText(QDir::currentPath()+"/");
}

void RecordScreen::OnClickRecord()
{
	
	if (isRecording == false)
	{
		AdjustInput();
		bool isScreen;
		if (ui.radioButton->isChecked())
		{
			isScreen = false;
		} 
		else
		{
			isScreen = true;
		}
		avrecord.init(   (char*)((ui.lineEdit_dir->text()+ui.lineEdit_filename->text()).toStdString().c_str()), isScreen);
		avrecord.startRecord();

		ui.lineEdit_filename->setReadOnly(true);
		ui.toolButton->setDisabled(true);
		ui.radioButton->setDisabled(true);
	}
	else
	{
		avrecord.stop();
		avrecord.clear();
		ui.lineEdit_filename->setReadOnly(false);
		ui.toolButton->setDisabled(false);
		ui.radioButton->setDisabled(false);
	}
	isRecording = !isRecording;
	if (isRecording)
	{
		qtime.restart();
		ui.pushButton_record->setStyleSheet(stop_style);
	}
	else
	{
		ui.pushButton_record->setStyleSheet(start_style);
	}
	cout << "ClickRecord" << endl;
}

void RecordScreen::OnClickDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择生成路径"));
	ui.lineEdit_dir->setText(dir+"/");
}

void RecordScreen::timerEvent(QTimerEvent *event)
{
	char buf[1024] = { 0 };
	if (isRecording)
	{
		int es = qtime.elapsed() / 1000;
		sprintf(buf, "%03d:%02d", es / 60, es % 60);
	}
	else
	{
		sprintf(buf, "000:00");
	}
	ui.label_time->setText(buf);
	
}


void RecordScreen::AdjustInput()
{
	if (ui.lineEdit_dir->text().isEmpty())
	{
		ui.lineEdit_dir->setText(QDir::currentPath()+"/");
	}
	if (ui.lineEdit_filename->text().isEmpty())
	{
		ui.lineEdit_filename->setText("test.mp4");
	}
	else
	{
		QString src = ui.lineEdit_filename->text();
		QString houZhui = src.mid(src.length() - 4);
		if (houZhui != QString(".mp4"))
		{
			ui.lineEdit_filename->setText(src+".mp4");
		}
	}
	

}