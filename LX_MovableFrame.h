#pragma once

#include <QFrame>

//���������,ʵ����
//����ô���,ʹ�丸����������ƶ�


class LX_MovableFrame : public QFrame
{
	Q_OBJECT

public:
	LX_MovableFrame(QObject *parent);
	~LX_MovableFrame();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	bool m_move = false;
	QPoint m_startPoint;
	QPoint m_windowPoint;

	QWidget*parent_widget;
};
