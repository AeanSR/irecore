#include "irecore.h"
#include "gic.h"

#define API_KEY "nyv354w3rnf3jf95ryn5ddmyar933pzw"
const double ilvlScaleCoeff = 1.009357190938255;

rapidjson::Document getjson::get(){
	reply = qnam.get(QNetworkRequest(url));

	QEventLoop loop;
	QApplication::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	QString doc(reply->readAll());
	rapidjson::Document j;

	if (reply->error()){
		QMessageBox::information(pg, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "Cannot connect Battle.Net:\n") + url.toString(), QMessageBox::Ok);
		return j;
	}

	j.Parse(doc.toStdString().c_str());

	return j;
}

QString qsprint(int v);

/*int gic::retrive_item_subclass(int id, std::string& region){
	getjson bn;
	QString url;
	char buf[32];
	bn.set_parent(this);
	if (region.compare("cn")){
		url = "https://us.api.battle.net/wow/item/";
	}
	else{
		url = "https://api.battlenet.com.cn/wow/item/";
	}
	url.append(itoa(id, buf, 10));
	url.append("?locale=en_US&apikey=");
	url.append(API_KEY);
	bn.set_url(url);
	rapidjson::Document j = bn.get();

	if (j["availableContexts"][0].GetStringLength()){
		if (region.compare("cn")){
			url = "https://us.api.battle.net/wow/item/";
		}
		else{
			url = "https://api.battlenet.com.cn/wow/item/";
		}
		url.append(itoa(id, buf, 10));
		url.append("/");
		url.append(j["availableContexts"][0].GetString());
		url.append("?locale=en_US&apikey=");
		url.append(API_KEY);
		bn.set_url(url);
		j = bn.get();
	}


	int itemClass = j["itemClass"].GetInt();
	qDebug() << url << " " << itemClass << " " << j["itemSubClass"].GetInt();

	if (itemClass == 4){
		return (j["itemSubClass"].GetInt() == 4) ? 0 : 1;
	}
	else if (itemClass == 2){
		switch (j["itemSubClass"].GetInt()){
		case 0: case 4: case 7: case 13: 
			return 1;
		case 1: case 5: case 6: case 8: case 10:
			return 0;
		case 15: default:
			return 2;
		}
	}
	else{
		return 0;
	}
}*/

// stat 4,72,74 -> strength
//      32 -> crit
//      36 -> haste
//      49 -> mastery
//      59 -> multistrike
//      40 -> versatility
void gic::import_player(std::string& realm, std::string& name, std::string& region){
	ui.btnImport->setDisabled(true);

	getjson bn;
	QString url;
	bn.set_parent(this);
	if (region.compare("cn")){
		url = "https://";
		url.append( region.c_str());
		url.append("api.battle.net/wow/character/");
		url.append(realm.c_str());
		url.append("/");
		url.append(name.c_str());
		url.append("?fields=talents,items&locale=en_US&apikey=");
		url.append(API_KEY);
	}
	else{
		url = "http://www.battlenet.com.cn/api/wow/character/";
		url.append(realm.c_str());
		url.append("/");
		url.append(name.c_str());
		url.append("?fields=talents,items&locale=en_US");
	}
	bn.set_url(url);
	rapidjson::Document j = bn.get();

	if (j["class"].GetInt() != 1){
		QMessageBox::information(this, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "This character is not a warrior."), QMessageBox::Ok);
		return;
	}
	if (j["level"].GetInt() != 100){
		QMessageBox::information(this, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "This character is not 100 lvl."), QMessageBox::Ok);
		return;
	}
	rapidjson::Value& jtalentlist = j["talents"];
	talent = -1;
	int glyph = 0;
	for (int i = 0; i < jtalentlist.Size(); i++){
		if (std::string("Z").compare(jtalentlist[i]["calcSpec"].GetString())) continue;
		std::string calcTalent = jtalentlist[i]["calcTalent"].GetString();
		talent = 0;
		for (int k = 0; k < 7; k++){
			talent *= 10;
			switch (calcTalent.length()>k ? calcTalent[k] : '.'){
			case '0': talent += 1; break;
			case '1': talent += 2; break;
			case '2': talent += 3; break;
			default: break;
			}
		}
		glyph = !!strchr(jtalentlist[i]["calcGlyph"].GetString(), 'c');
	}
	if (talent < 0){
		QMessageBox::information(this, QApplication::translate("gicClass", "Import Fail"), QApplication::translate("gicClass", "This character do not have fury spec."), QMessageBox::Ok);
		return;
	}
	switch (j["race"].GetInt()){
	case 1: race = 1; break;
	case 3: race = 2; break;
	case 7: race = 3; break;
	case 4: race = 4; break;
	case 11: race = 6; break;
	case 22: race = 7; break;
	case 2: race = 8; break;
	case 8: race = 9; break;
	case 6: race = 10; break;
	case 5: race = 11; break;
	case 10: race = 12; break;
	case 9: race = 13; break;
	case 24: case 25: case 26: race = 14; break;
	default: race = 0;
	}
	rapidjson::Value& items = j["items"];
	static const char* slotname[] = {
		"head", "neck", "shoulder", "back", "chest", "wrist", "mainHand", "offHand",
		"hands", "waist", "legs", "feet", "finger1", "finger2", "trinket1", "trinket2",
	};
	static const int plate_slot[] = {
		1, 0, 1, 0, 1, 1, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
	};
	int t17_count = 0;
	int t18_count = 0;
	archmages_greater_incandescence = 0;
	archmages_incandescence = 0;
	legendary_ring = 0;
	mh_low = 0;
	mh_high = 0;
	oh_low = 0;
	oh_high = 0;
	mh_speed = 0;
	oh_speed = 0;
	ui.comboTrinketSpecial1->setCurrentIndex(0);
	ui.comboTrinketSpecial2->setCurrentIndex(0);
	ui.txtTrinketValue1->clear();
	ui.txtTrinketValue2->clear();

	for (int i = 0; i < 16; i++){
		gear_list[i] = item_t();
		rapidjson::Value& item = items[slotname[i]];
		int itemid = item["id"].GetInt();
		int itemlvl = item["itemLevel"].GetInt();
		if (itemid == 0) continue;
		if (itemid >= 115580 && itemid <= 115584) t17_count++;
		if (itemid == 124319 || itemid == 124329 || itemid == 124334 || itemid == 124340 || itemid == 124346) t18_count++;
		if (itemid == 118305) archmages_greater_incandescence = 1;
		if (itemid == 118300) archmages_incandescence = 1;
		if (itemid == 124634){
			legendary_ring = 2500.0 * pow(ilvlScaleCoeff, itemlvl - 735);
		}
		/* 	"none",0
	"vial_of_convulsive_shadows",1
	"forgemasters_insignia",2
	"horn_of_screaming_spirits",3
	"scabbard_of_kyanos",4
	"badge_of_victory",5
	"insignia_of_victory",6
	"tectus_beating_heart",7
	"formidable_fang",8
	"draenic_stone",9
	"skull_of_war",10
	"mote_of_the_mountain",11
	"worldbreakers_resolve",12 */
		if (i == 14 || i == 15){ /* Trinket setup */
			QComboBox* combo = (i == 14) ? ui.comboTrinketSpecial1 : ui.comboTrinketSpecial2;
			QLineEdit* input = (i == 14) ? ui.txtTrinketValue1 : ui.txtTrinketValue2;
			if (itemid == 113969){
				combo->setCurrentIndex(1);
				switch (itemlvl){
				case 670: input->setText(qsprint(1537)); break;
				case 676: input->setText(qsprint(1627)); break;
				case 685: input->setText(qsprint(1767)); break;
				case 691: input->setText(qsprint(1870)); break;
				case 700: input->setText(qsprint(2033)); break;
				case 706: input->setText(qsprint(2150)); break;
				default:  input->setText(qsprint((int)(2033.0 * pow(ilvlScaleCoeff, itemlvl - 700)))); break;
				}
			}
			if (itemid == 113983){
				combo->setCurrentIndex(2);
				switch (itemlvl){
				case 670: input->setText(qsprint(137)); break;
				case 676: input->setText(qsprint(145)); break;
				case 685: input->setText(qsprint(157)); break;
				case 691: input->setText(qsprint(167)); break;
				case 700: input->setText(qsprint(181)); break;
				case 706: input->setText(qsprint(192)); break;
				default:  input->setText(qsprint((int)(181.0 * pow(ilvlScaleCoeff, itemlvl - 700)))); break;
				}
			}
			if (itemid == 119193){
				combo->setCurrentIndex(3);
				switch (itemlvl){
				case 670: input->setText(qsprint(2004)); break;
				case 676: input->setText(qsprint(2122)); break;
				case 685: input->setText(qsprint(2304)); break;
				case 691: input->setText(qsprint(2439)); break;
				case 700: input->setText(qsprint(2652)); break;
				case 706: input->setText(qsprint(2804)); break;
				default:  input->setText(qsprint((int)(2652.0 * pow(ilvlScaleCoeff, itemlvl - 700)))); break;
				}
			}
			if (itemid == 118882){
				combo->setCurrentIndex(4);
				switch (itemlvl){
				case 665: input->setText(qsprint(2200)); break;
				default:  input->setText(qsprint((int)(2200.0 * pow(ilvlScaleCoeff, itemlvl - 665)))); break;
				}
			}
			if (itemid == 115159 || itemid == 119936 || itemid == 111232 || itemid == 115759 || itemid == 125041 || itemid == 125518 || itemid == 126632 || itemid == 126155 || itemid == 124867 || itemid == 125344 || itemid == 125981 || itemid == 126458){
				combo->setCurrentIndex(5);
				switch (itemlvl){
				case 620: input->setText(qsprint(456)); break;
				case 626: input->setText(qsprint(483)); break;
				case 660: input->setText(qsprint(728)); break;
				case 670: input->setText(qsprint(727)); break;
				case 680: input->setText(qsprint(798)); break;
				case 700: input->setText(qsprint(1057)); break;
				case 705: input->setText(qsprint(1108)); break;
				default:  input->setText(qsprint((int)(728.0 * pow(ilvlScaleCoeff, itemlvl - 660)))); break;
				}
			}
			if (itemid == 115160 || itemid == 119937 || itemid == 111233 || itemid == 115760 || itemid == 125042 || itemid == 125519 || itemid == 126633 || itemid == 126156 || itemid == 124868 || itemid == 125345 || itemid == 125982 || itemid == 126459){
				combo->setCurrentIndex(6);
				switch (itemlvl){
				case 620: input->setText(qsprint(543)); break;
				case 626: input->setText(qsprint(575)); break;
				case 660: input->setText(qsprint(867)); break;
				case 670: input->setText(qsprint(866)); break;
				case 680: input->setText(qsprint(951)); break;
				case 700: input->setText(qsprint(1259)); break;
				case 705: input->setText(qsprint(1319)); break;
				default:  input->setText(qsprint((int)(867.0 * pow(ilvlScaleCoeff, itemlvl - 660)))); break;
				}
			}
			if (itemid == 113645){
				combo->setCurrentIndex(7);
				switch (itemlvl){
				case 655: input->setText(qsprint(1743)); break;
				case 661: input->setText(qsprint(1843)); break;
				case 670: input->setText(qsprint(2004)); break;
				case 676: input->setText(qsprint(2122)); break;
				case 685: input->setText(qsprint(2304)); break;
				case 691: input->setText(qsprint(2439)); break;
				default:  input->setText(qsprint((int)(2304.0 * pow(ilvlScaleCoeff, itemlvl - 685)))); break;
				}
			}
			if (itemid == 114613){
				combo->setCurrentIndex(8);
				switch (itemlvl){
				case 655: input->setText(qsprint(1743)); break;
				default:  input->setText(qsprint((int)(1743.0 * pow(ilvlScaleCoeff, itemlvl - 655)))); break;
				}
			}
			if (itemid == 109262 || (itemid >= 122601 && itemid <= 122604)){
				combo->setCurrentIndex(9);
				switch (itemlvl){
				case 620: input->setText(qsprint(771)); break;
				case 640: input->setText(qsprint(931)); break;
				case 655: input->setText(qsprint(1069)); break;
				case 670: input->setText(qsprint(1229)); break;
				case 685: input->setText(qsprint(1414)); break;
				default:  input->setText(qsprint((int)(1414.0 * pow(ilvlScaleCoeff, itemlvl - 685)))); break;
				}
			}
			if (itemid == 112318){
				combo->setCurrentIndex(10);
				switch (itemlvl){
				case 640: input->setText(qsprint(1396)); break;
				case 655: input->setText(qsprint(1604)); break;
				case 670: input->setText(qsprint(1844)); break;
				case 685: input->setText(qsprint(2120)); break;
				case 700: input->setText(qsprint(2440)); break;
				case 715: input->setText(qsprint(2804)); break;
				default:  input->setText(qsprint((int)(2804.0 * pow(ilvlScaleCoeff, itemlvl - 715)))); break;
				}
			}
			if (itemid == 116292){
				combo->setCurrentIndex(11);
				switch (itemlvl){
				case 640: input->setText(qsprint(1517)); break;
				case 646: input->setText(qsprint(1604)); break;
				default:  input->setText(qsprint((int)(1517.0 * pow(ilvlScaleCoeff, itemlvl - 640)))); break;
				}
			}
			if (itemid == 124523){
				combo->setCurrentIndex(12);
				switch (itemlvl){
				case 705: input->setText(qsprint(220)); break;
				case 711: input->setText(qsprint(232)); break;
				case 720: input->setText(qsprint(253)); break;
				case 726: input->setText(qsprint(267)); break;
				case 735: input->setText(qsprint(290)); break;
				case 741: input->setText(qsprint(307)); break;
				default:  input->setText(qsprint((int)(290.0 * pow(ilvlScaleCoeff, itemlvl - 735)))); break;
				}
			}
			if (itemid == 124237){
				combo->setCurrentIndex(13);
				switch (itemlvl){
				case 700: input->setText(qsprint(20564)); break;
				case 706: input->setText(qsprint(21744)); break;
				case 715: input->setText(qsprint(23632)); break;
				case 721: input->setText(qsprint(24981)); break;
				case 730: input->setText(qsprint(27172)); break;
				case 736: input->setText(qsprint(28757)); break;
				default:  input->setText(qsprint((int)(27172.0 * pow(ilvlScaleCoeff, itemlvl - 730)))); break;
				}
			}
			if (itemid == 124238){
				combo->setCurrentIndex(14);
				switch (itemlvl){
				case 700: input->setText(qsprint(259)); break;
				case 706: input->setText(qsprint(274)); break;
				case 715: input->setText(qsprint(298)); break;
				case 721: input->setText(qsprint(315)); break;
				case 730: input->setText(qsprint(342)); break;
				case 736: input->setText(qsprint(362)); break;
				default:  input->setText(qsprint((int)(342.0 * pow(ilvlScaleCoeff, itemlvl - 730)))); break;
				}
			}
			if (itemid == 124236){
				combo->setCurrentIndex(15);
				switch (itemlvl){
				case 695: input->setText(qsprint(54)); break;
				case 701: input->setText(qsprint(57)); break;
				case 710: input->setText(qsprint(62)); break;
				case 716: input->setText(qsprint(65)); break;
				case 725: input->setText(qsprint(71)); break;
				case 731: input->setText(qsprint(75)); break;
				default:  input->setText(qsprint((int)(71.0 * pow(ilvlScaleCoeff, itemlvl - 725)))); break;
				}
			}
		}
		rapidjson::Value& piecestat = item["stats"];
		for (int k = 0; k < piecestat.Size(); k++){
			switch (piecestat[k]["stat"].GetInt()){
			case 4: case 72: case 74: gear_list[i].str += piecestat[k]["amount"].GetInt(); break;
			case 32: gear_list[i].crit += piecestat[k]["amount"].GetInt(); break;
			case 36: gear_list[i].haste += piecestat[k]["amount"].GetInt(); break;
			case 49: gear_list[i].mastery += piecestat[k]["amount"].GetInt(); break;
			case 59: gear_list[i].mult += piecestat[k]["amount"].GetInt(); break;
			case 40: gear_list[i].vers += piecestat[k]["amount"].GetInt(); break;
			default:break;
			}
		}
		rapidjson::Value& pieceench = item["tooltipParams"];
		int gemid = pieceench["gem0"].GetInt();
		if (gemid){
			enchant_t* ench = gem_list;
			while (ench->id){
				if (ench->id == gemid) break;
				ench++;
			}
			switch (ench->stat){
			case 4: case 72: case 74: gear_list[i].str += ench->value; break;
			case 32: gear_list[i].crit += ench->value; break;
			case 36: gear_list[i].haste += ench->value; break;
			case 49: gear_list[i].mastery += ench->value; break;
			case 59: gear_list[i].mult += ench->value; break;
			case 40: gear_list[i].vers += ench->value; break;
			default:break;
			}
		}
		int enchid = pieceench["enchant"].GetInt();
		if (i == 6){
			if (enchid == 5330) ui.comboMHEnchant->setCurrentIndex(1);
			else if (enchid == 5384) ui.comboMHEnchant->setCurrentIndex(2);
			else if (enchid == 5331) ui.comboMHEnchant->setCurrentIndex(3);
			else ui.comboMHEnchant->setCurrentIndex(0);
			mh_low = item["weaponInfo"]["damage"]["min"].GetInt();
			mh_high = item["weaponInfo"]["damage"]["max"].GetInt();
			mh_speed = item["weaponInfo"]["weaponSpeed"].GetDouble();
			gear_list[i].type = 2;
			for (auto p = weapon_type_list; p->id; p++){
				if (p->id == itemid){
					switch (p->subclass){
					case 0: case 4: case 7: case 13:
						gear_list[i].type = 1; break;
					case 1: case 5: case 6: case 8: case 10:
						gear_list[i].type = 0; break;
					case 15: default:
						gear_list[i].type = 2; break;
					}
					break;
				}
			}
		}
		if (i == 7){
			if (enchid == 5330) ui.comboOHEnchant->setCurrentIndex(1);
			else if (enchid == 5384) ui.comboOHEnchant->setCurrentIndex(2);
			else if (enchid == 5331) ui.comboOHEnchant->setCurrentIndex(3);
			else ui.comboOHEnchant->setCurrentIndex(0);
			oh_low = item["weaponInfo"]["damage"]["min"].GetInt();
			oh_high = item["weaponInfo"]["damage"]["max"].GetInt();
			oh_speed = item["weaponInfo"]["weaponSpeed"].GetDouble();
			gear_list[i].type = 2;
			for (auto p = weapon_type_list; p->id; p++){
				if (p->id == itemid){
					switch (p->subclass){
					case 0: case 4: case 7: case 13:
						gear_list[i].type = 1; break;
					case 1: case 5: case 6: case 8: case 10:
						gear_list[i].type = 0; break;
					case 15: default:
						gear_list[i].type = 2; break;
					}
					break;
				}
			}
		}
		if (enchid){
			enchant_t* ench = enchant_list;
			while (ench->id){
				if (ench->id == enchid) break;
				ench++;
			}
			switch (ench->stat){
			case 4: case 72: case 74: gear_list[i].str += ench->value; break;
			case 32: gear_list[i].crit += ench->value; break;
			case 36: gear_list[i].haste += ench->value; break;
			case 49: gear_list[i].mastery += ench->value; break;
			case 59: gear_list[i].mult += ench->value; break;
			case 40: gear_list[i].vers += ench->value; break;
			default:break;
			}
		}
		if (plate_slot[i]){
			gear_list[i].type = 1;
			for (int* p = plate_list; *p; p++){
				if (*p == itemid){
					gear_list[i].type = 0;
					break;
				}
			}
		}
	}

	ui.comboRace->setCurrentIndex(race);
	select_gear_slot();
	gear_summary_calculate();
	if (legendary_ring){ ui.comboIncandescence->setCurrentIndex(3); ui.txtLegendaryRing->setText(qsprint(legendary_ring)); }
	else if (archmages_greater_incandescence) ui.comboIncandescence->setCurrentIndex(2);
	else if (archmages_incandescence) ui.comboIncandescence->setCurrentIndex(1);
	else ui.comboIncandescence->setCurrentIndex(0);
	ui.checkT172P->setChecked(t17_count >= 2);
	ui.checkT174P->setChecked(t17_count >= 4);
	ui.checkT182P->setChecked(t18_count >= 2);
	ui.checkT184P->setChecked(t18_count >= 4);
	ui.txtMHHigh->setText(qsprint(mh_high));
	ui.txtMHLow->setText(qsprint(mh_low));
	char buf[32];
	sprintf(buf, "%.2f", mh_speed);
	ui.txtMHSpeed->setText(buf);
	ui.txtOHHigh->setText(qsprint(oh_high));
	ui.txtOHLow->setText(qsprint(oh_low));
	sprintf(buf, "%.2f", oh_speed);
	ui.txtOHSpeed->setText(buf);

	ui.comboTalent7->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent6->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent5->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent4->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent3->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent2->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.comboTalent1->setCurrentIndex(talent % 10);
	talent /= 10;
	ui.checkGlyphOfUnendingRage->setChecked(glyph);
	ui.btnImport->setDisabled(false);
}