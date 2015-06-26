#ifndef GIC_H
#define GIC_H

#include <QtWidgets/QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include "ui_gic.h"
#include <QtNetwork>
#include <qthread.h>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtWidgets>
#include "rapidjson\document.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\prettywriter.h"



class gic : public QMainWindow
{
	Q_OBJECT

public:
	gic(QWidget *parent = 0);
	~gic();
	void reset_result_page();
	void run_simulation();
	void import_player(std::string& realm, std::string& name, std::string& region);
	int retrive_item_subclass(int id, std::string& region);
private slots:
    void on_btnRun_clicked();
	void on_btnImport_clicked();
	void on_listActions_itemDoubleClicked();
	void on_listConditions_itemDoubleClicked();
	void TxtBoxNotify(QString);
	void mh_dps_calculate();
	void oh_dps_calculate();
	void gear_summary_calculate();
	void select_gear_slot();
private:
	Ui::gicClass ui;
	
};

class getjson{

private:
	QUrl url;
	QNetworkAccessManager qnam;
	QNetworkReply *reply;
	gic* pg;
public:
	void set_url(QString& surl){
		url.setUrl(surl, QUrl::ParsingMode::TolerantMode);
	}
	void set_parent(gic* _pg){
		pg = _pg;
	}
	rapidjson::Document get();
};


#endif // GIC_H
