#include "LX_MovableFrame.h"
#include <QMouseEvent>
LX_MovableFrame::LX_MovableFrame(QObject *parent)
	: QFrame((QFrame*)parent)
{
	parent_widget = (QWidget*)parent;
}

LX_MovableFrame::~LX_MovableFrame()
{
}

void LX_MovableFrame::mousePressEvent(QMouseEvent *event)
{
	//当鼠标左键点击时.
	if (event->button() == Qt::LeftButton)
	{
		m_move = true;
		//记录鼠标的世界坐标.
		m_startPoint = event->globalPos();
		//记录窗体的世界坐标.
		m_windowPoint = parent_widget->frameGeometry().topLeft();
	}

}
void LX_MovableFrame::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		//移动中的鼠标位置相对于初始位置的相对位置.
		QPoint relativePos = event->globalPos() - m_startPoint;
		//然后移动窗体即可.
		//this->move(m_windowPoint + relativePos);
		parent_widget->move(m_windowPoint + relativePos);
	}
}
void LX_MovableFrame::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		//改变移动状态.
		m_move = false;
	}
}