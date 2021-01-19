#pragma once

#include <QFrame>

//这个窗口类,实现了
//点击该窗口,使其父窗口随鼠标移动


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
