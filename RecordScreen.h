#pragma once

#include <QtWidgets/QWidget>
#include "ui_RecordScreen.h"
#include "LX_AVRecord.h"

class RecordScreen : public QWidget
{
	Q_OBJECT
public:
	RecordScreen(QWidget *parent = Q_NULLPTR);
	void timerEvent(QTimerEvent *event);

public slots:
	void OnClickRecord();
	void OnClickDir();
private:
	Ui::RecordScreenClass ui;
	bool isRecording = false;
	bool isRunning = false;
	LX_AVRecord avrecord;

	void AdjustInput();
};
