#include "BackendGuI.h"
#include <QDebug>
BackendGuI::BackendGuI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.accountBtn->setStyleSheet("color:red");

	yearPlanButton->setGeometry(10, 10, 200, 200);

	connect(ui.accountBtn, SIGNAL(clicked()), this, SLOT(onAccountBtnclicked()));

}
/*--------------------------------------------------------------------
** ���� : onAccountBtnclicked
**--------------------------------------------------------------------
** ���� : �¼���Ӧ�ۺ���
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 18.06.28		��ΰ
**-------------------------------------------------------------------*/
void BackendGuI::onAccountBtnclicked()
{
	qDebug() << "..";
}