#ifndef GIC_H
#define GIC_H

#include <QtWidgets/QMainWindow>
#include "ui_gic.h"

class gic : public QMainWindow
{
	Q_OBJECT

public:
	gic(QWidget *parent = 0);
	~gic();
	void reset_result_page();

private slots:
    void on_btnRun_clicked();

private:
	Ui::gicClass ui;
};

#endif // GIC_H
