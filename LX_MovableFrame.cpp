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
	//�����������ʱ.
	if (event->button() == Qt::LeftButton)
	{
		m_move = true;
		//��¼������������.
		m_startPoint = event->globalPos();
		//��¼�������������.
		m_windowPoint = parent_widget->frameGeometry().topLeft();
	}

}
void LX_MovableFrame::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		//�ƶ��е����λ������ڳ�ʼλ�õ����λ��.
		QPoint relativePos = event->globalPos() - m_startPoint;
		//Ȼ���ƶ����弴��.
		//this->move(m_windowPoint + relativePos);
		parent_widget->move(m_windowPoint + relativePos);
	}
}
void LX_MovableFrame::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		//�ı��ƶ�״̬.
		m_move = false;
	}
}