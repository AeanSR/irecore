#include "gic.h"
#include "VersionNo.h"

gic::gic(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QString ver_str(QApplication::translate("gicClass","  Current Version: ", 0));
	ver_str.append(STRFILEVER);
	ui.lblVersion->setText(ver_str);
}

gic::~gic()
{

}

void gic::on_btnRun_clicked()
{

}
