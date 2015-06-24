#ifndef GIC_H
#define GIC_H

#include <QtWidgets/QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include "ui_gic.h"

class gic : public QMainWindow
{
	Q_OBJECT

public:
	gic(QWidget *parent = 0);
	~gic();
	void reset_result_page();
	void run_simulation();
signals:
	void more_result(const QString&);

private slots:
    void on_btnRun_clicked();
	void on_listActions_itemDoubleClicked();
	void on_listConditions_itemDoubleClicked();
	void TxtBoxNotify(QString);
private:
	Ui::gicClass ui;
	
};

#endif // GIC_H
