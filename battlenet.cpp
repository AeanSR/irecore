#include "irecore.h"
#include "gic.h"


rapidjson::Document getjson::get(){
	reply = qnam.get(QNetworkRequest(url));

	QEventLoop loop;
	QApplication::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	QString doc(reply->readAll());
	rapidjson::Document j;

	if (reply->error()){
		QMessageBox::information(pg, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "Cannot connect Battle Net."), QMessageBox::Ok);
		return j;
	}

	j.Parse(doc.toStdString().c_str());

	return j;
}

void import_player(gic* pg, std::string& realm, std::string& name, std::string& region){
	getjson bn;
	QString url;
	bn.set_parent(pg);
	if (region.compare("cn")){
		url = "http://";
		url.append( region.c_str());
		url.append(".battle.net/api/wow/character/");
		url.append(realm.c_str());
		url.append("/");
		url.append(name.c_str());
		url.append("?fields=talents,items&locale=en_US");
	}
	else{
		url = "http://www.battlenet.com.cn/api/wow/character/";
		url.append(realm.c_str());
		url.append("/");
		url.append(name.c_str());
		url.append("?fields=talents,items&locale=en_US");
	}
	qDebug() << url;
	bn.set_url(url);
	rapidjson::Document j = bn.get();
	rapidjson::StringBuffer b;
	rapidjson::PrettyWriter< rapidjson::StringBuffer > writer(b);
	j.Accept(writer);
	qDebug() << b.GetString();

	if (j["class"].GetInt() != 1){
		QMessageBox::information(pg, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "This character is not a warrior."), QMessageBox::Ok);
		return;
	}

}