#include "gic.h"
#include "VersionNo.h"
#include "irecore.h"

void set_default_parameters();

item_t gear_list[16];
int selected_gear_slot = 0;
QString gear_type_list_misc[] = {
	QString(),
};
QString gear_type_list_armor[] = {
	QString(),
	QString(),
	QString(),
};
QString gear_type_list_weapon[] = {
	QString(),
	QString(),
	QString(),
	QString(),
};
QString* gear_type_list[] = {
	gear_type_list_armor,
	gear_type_list_misc,
	gear_type_list_armor,
	gear_type_list_misc,
	gear_type_list_armor,
	gear_type_list_armor,
	gear_type_list_weapon,
	gear_type_list_weapon,
	gear_type_list_armor,
	gear_type_list_armor,
	gear_type_list_armor,
	gear_type_list_armor,
	gear_type_list_misc,
	gear_type_list_misc,
	gear_type_list_misc,
	gear_type_list_misc,
};

void gic::reset_result_page(){
	ui.lblDPS->setText("N/A");
	ui.lblDPSRange->setText("N/A");
	ui.lblDPSError->setText("N/A");

	ui.lblWeight->setText("");
	ui.barWeight->setGeometry(QRect(500, 500, 1, 1));
	ui.barError->setGeometry(QRect(500, 500, 1, 1));

	ui.lblWeight_2->setText("");
	ui.barWeight_2->setGeometry(QRect(500, 500, 1, 1));
	ui.barError_2->setGeometry(QRect(500, 500, 1, 1));

	ui.lblWeight_3->setText("");
	ui.barWeight_3->setGeometry(QRect(500, 500, 1, 1));
	ui.barError_3->setGeometry(QRect(500, 500, 1, 1));

	ui.lblWeight_4->setText("");
	ui.barWeight_4->setGeometry(QRect(500, 500, 1, 1));
	ui.barError_4->setGeometry(QRect(500, 500, 1, 1));

	ui.lblWeight_5->setText("");
	ui.barWeight_5->setGeometry(QRect(500, 500, 1, 1));
	ui.barError_5->setGeometry(QRect(500, 500, 1, 1));

	ui.lblWeight_6->setText("");
	ui.barWeight_6->setGeometry(QRect(500, 500, 1, 1));
	ui.barError_6->setGeometry(QRect(500, 500, 1, 1));
}

gic::gic(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	qRegisterMetaType<QString>("QString");
	reset_result_page();
	ui.tabWidget->setCurrentIndex(0);

	gear_type_list_armor[0] = QApplication::translate("gicClass", "Plate");
	gear_type_list_armor[1] = QApplication::translate("gicClass", "Mail/Leather/Cloth");
	gear_type_list_weapon[0] = QApplication::translate("gicClass", "2H Sword/Axe/Mace/Polearm");
	gear_type_list_weapon[1] = QApplication::translate("gicClass", "1H Sword/Axe/Mace/Fist");
	gear_type_list_weapon[2] = QApplication::translate("gicClass", "Dagger");

	// Show current version.
	QString ver_str(QApplication::translate("gicClass", "  Current Version: ", 0));
	ver_str.append(STRFILEVER);
	ver_str.append(" ");
	ver_str.append(__DATE__);
	ui.lblVersion->setText(ver_str);

	// Query device list.
	cl_int err;
	cl_uint num;
	std::vector<cl_platform_id> platforms;
	err = clGetPlatformIDs(0, 0, &num);
	if (err != CL_SUCCESS) {
		std::cerr << "Unable to get platforms\n";
		abort();
	}
	platforms.resize(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if (err != CL_SUCCESS) {
		std::cerr << "Unable to get platform ID\n";
		abort();
	}
	int device_counter = 0;
	for (size_t platform_id = 0; platform_id < num; platform_id++){
		size_t dev_c, info_c;
		clGetPlatformInfo(platforms[platform_id], CL_PLATFORM_NAME, 0, NULL, &info_c);
		cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[platform_id]), 0 };
		cl_context context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
		if (context == 0) {
			std::cerr << "Can't create OpenCL context\n";
			abort();
		}
		clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dev_c);
		std::vector<cl_device_id> devices(dev_c / sizeof(cl_device_id));
		clGetContextInfo(context, CL_CONTEXT_DEVICES, dev_c, &devices[0], 0);
		for (auto i = devices.begin(); i != devices.end(); i++){
			clGetDeviceInfo(*i, CL_DEVICE_NAME, 0, NULL, &info_c);
			std::string devname;
			devname.resize(info_c);
			clGetDeviceInfo(*i, CL_DEVICE_NAME, info_c, &devname[0], 0);
			char device_name[512];
			sprintf(device_name, "Device %d: %s", device_counter, devname.c_str());
			ui.comboDevices->addItem(device_name, device_counter);
			device_counter++;
			pdpair_t pd;
			pd.device_id = i - devices.begin();
			pd.platform_id = platform_id;
			ocl_device_list.push_back(pd);
		}
		clReleaseContext(context);
	}

	// Add RNG Engines.
	ui.comboRNG->addItem("LCG32", 32);
	ui.comboRNG->addItem("MWC64X", 64);
	ui.comboRNG->addItem("MT127", 127);

	// Add iterations.
	ui.comboIterations->addItem("1", 1);
	ui.comboIterations->addItem("2,000", 2000);
	ui.comboIterations->addItem("10,000", 10000);
	ui.comboIterations->addItem("50,000", 50000);
	ui.comboIterations->addItem("100,000", 100000);
	ui.comboIterations->addItem("250,000", 250000);
	ui.comboIterations->addItem("1,000,000", 1000000);
	ui.comboIterations->setCurrentIndex(3);

	// Combat length.
	ui.comboCombatLength->addItem("30", 30);
	ui.comboCombatLength->addItem("120", 120);
	ui.comboCombatLength->addItem("180", 180);
	ui.comboCombatLength->addItem("210", 210);
	ui.comboCombatLength->addItem("240", 240);
	ui.comboCombatLength->addItem("270", 270);
	ui.comboCombatLength->addItem("300", 300);
	ui.comboCombatLength->addItem("330", 330);
	ui.comboCombatLength->addItem("450", 450);
	ui.comboCombatLength->addItem("600", 600);
	ui.comboCombatLength->addItem("720", 720);
	ui.comboCombatLength->addItem("900", 900);
	ui.comboCombatLength->setCurrentIndex(8);

	// Vary combat length.
	ui.comboVaryCombatLength->addItem("20", 20.0f);
	ui.comboVaryCombatLength->addItem("10", 10.0f);
	ui.comboVaryCombatLength->addItem("0", 0.0f);

	// Init health pct.
	ui.comboInitialHealthPercentage->addItem("100", 100.0f);
	ui.comboInitialHealthPercentage->addItem("19", 19.0f);

	// Death pct.
	ui.comboDeathPct->addItem("0", 0.0f);
	ui.comboDeathPct->addItem("10", 10.0f);
	ui.comboDeathPct->addItem("21", 21.0f);

	// Policy Action List.
	QStringList lists;
	lists << "SPELL(bloodthirst);"
		<< "SPELL(ragingblow);"
		<< "SPELL(wildstrike);"
		<< "SPELL(potion);"
		<< "SPELL(berserkerrage);"
		<< "SPELL(recklessness);"
		<< "SPELL(stormbolt);"
		<< "SPELL(shockwave);"
		<< "SPELL(dragonroar);"
		<< "SPELL(ravager);"
		<< "SPELL(siegebreaker);"
		<< "SPELL(bladestorm);"
		<< "SPELL(avatar);"
		<< "SPELL(bloodbath);"
		<< "SPELL(arcanetorrent);"
		<< "SPELL(berserking);"
		<< "SPELL(bloodfury);"
		<< "SPELL(thorasus_the_stone_heart_of_draenor);"
		<< "SPELL(vial_of_convulsive_shadows);"
		<< "SPELL(scabbard_of_kyanos);"
		<< "SPELL(badge_of_victory);"
		<< "SPELL(bonemaws_big_toe);"
		<< "SPELL(emberscale_talisman);";
	ui.listActions->addItems(lists);
	lists.clear();
	lists << "FROM_SECONDS()"
		<< "enemy_health_percent(rti)"
		<< "rti->player.power"
		<< "power_max"
		<< "TIME_DISTANT(rti->expected_combat_length)"
		<< "UP(bloodthirst.cd)"
		<< "REMAIN(bloodthirst.cd)"
		<< "rti->player.ragingblow.stack"
		<< "REMAIN(ragingblow.expire)"
		<< "REMAIN(enrage.expire)"
		<< "rti->player.bloodsurge.stack"
		<< "REMAIN(bloodsurge.expire)"
		<< "UP(suddendeath.expire)"
		<< "REMAIN(suddendeath.expire)"
		<< "REMAIN(berserkerrage.cd)"
		<< "UP(recklessness.expire)"
		<< "REMAIN(recklessness.expire)"
		<< "REMAIN(recklessness.cd)"
		<< "REMAIN(stormbolt.cd)"
		<< "REMAIN(shockwave.cd)"
		<< "REMAIN(dragonroar.cd)"
		<< "UP(avatar.expire)"
		<< "REMAIN(avatar.expire)"
		<< "REMAIN(avatar.cd)"
		<< "UP(bloodbath.expire)"
		<< "REMAIN(bloodbath.expire)"
		<< "REMAIN(bloodbath.cd)"
		<< "UP(bladestorm.expire)"
		<< "REMAIN(bladestorm.expire)"
		<< "REMAIN(bladestorm.cd)"
		<< "UP(ravager.expire)"
		<< "REMAIN(ravager.expire)"
		<< "REMAIN(ravager.cd)"
		<< "REMAIN(siegebreaker.cd)"
		<< "UP(potion.expire)"
		<< "REMAIN(potion.expire)"
		<< "REMAIN(potion.cd)"
		<< "UP(thorasus_the_stone_heart_of_draenor.expire)"
		<< "REMAIN(thorasus_the_stone_heart_of_draenor.expire)"
		<< "REMAIN(thorasus_the_stone_heart_of_draenor.cd)";
	ui.listConditions->addItems(lists);
	lists.clear();
	auto_apl();
	ui.txtAPL->setPlainText(QString(apl.c_str()));

	ui.comboRegion->addItem("cn");
	ui.comboRegion->addItem("us");
	ui.comboRegion->addItem("eu");
	ui.comboRegion->addItem("tw");
	ui.comboRegion->addItem("kr");

	connect(ui.radioHelm, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioNeck, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioShoulder, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioBack, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioChest, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioWrist, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioMainhand, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioOffhand, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioHand, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioWaist, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioLeg, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioFeet, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioFinger1, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioFinger2, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioTrinket1, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));
	connect(ui.radioTrinket2, SIGNAL(toggled(bool)), this, SLOT(select_gear_slot()));

	connect(ui.txtItemStr, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.txtItemCrit, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.txtItemHaste, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.txtItemMastery, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.txtItemMult, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.txtItemVers, SIGNAL(textEdited(const QString &)), this, SLOT(gear_summary_calculate()));
	connect(ui.comboItemType, SIGNAL(activated(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.comboRace, SIGNAL(activated(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffAP, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffStr, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffCrit, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffMult, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffVers, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffHaste, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffMastery, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffFlask, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));
	connect(ui.checkRaidBuffFood, SIGNAL(stateChanged(int)), this, SLOT(gear_summary_calculate()));

	ui.comboIncandescence->addItem(QApplication::translate("gicClass", "No Legendary Ring"));
	ui.comboIncandescence->addItem(QApplication::translate("gicClass", "Incandescence(690)"));
	ui.comboIncandescence->addItem(QApplication::translate("gicClass", "Greater Incandescence(715)"));
	ui.comboIncandescence->addItem(QApplication::translate("gicClass", "Thorasus(735~795)"));
	ui.txtLegendaryRing->setEnabled(false);

	ui.comboMHEnchant->addItem(QApplication::translate("gicClass", "No Enchantment."));
	ui.comboOHEnchant->addItem(QApplication::translate("gicClass", "No Enchantment."));
	ui.comboMHEnchant->addItem(QApplication::translate("gicClass", "Thunderlord"));
	ui.comboOHEnchant->addItem(QApplication::translate("gicClass", "Thunderlord"));
	ui.comboMHEnchant->addItem(QApplication::translate("gicClass", "Bleeding Hollow"));
	ui.comboOHEnchant->addItem(QApplication::translate("gicClass", "Bleeding Hollow"));
	ui.comboMHEnchant->addItem(QApplication::translate("gicClass", "Shattered Hand"));
	ui.comboOHEnchant->addItem(QApplication::translate("gicClass", "Shattered Hand"));

	connect(ui.txtMHHigh, SIGNAL(textEdited(const QString &)), this, SLOT(mh_dps_calculate()));
	connect(ui.txtMHLow, SIGNAL(textEdited(const QString &)), this, SLOT(mh_dps_calculate()));
	connect(ui.txtMHSpeed, SIGNAL(textEdited(const QString &)), this, SLOT(mh_dps_calculate()));
	connect(ui.txtOHHigh, SIGNAL(textEdited(const QString &)), this, SLOT(oh_dps_calculate()));
	connect(ui.txtOHLow, SIGNAL(textEdited(const QString &)), this, SLOT(oh_dps_calculate()));
	connect(ui.txtOHSpeed, SIGNAL(textEdited(const QString &)), this, SLOT(oh_dps_calculate()));

	lists <<
		QApplication::translate("gicClass", "No Trinket Special.") <<
		QApplication::translate("gicClass", "Vial of Convulsive Shadows") <<
		QApplication::translate("gicClass", "Forgemaster's Insignia") <<
		QApplication::translate("gicClass", "Horn of Screaming Spirits") <<
		QApplication::translate("gicClass", "Scabbard of Kyanos") <<
		QApplication::translate("gicClass", "Badge of Victory") <<
		QApplication::translate("gicClass", "Insignia of Victory") <<
		QApplication::translate("gicClass", "Tectus' Beating Heart") <<
		QApplication::translate("gicClass", "Formidable Fang") <<
		QApplication::translate("gicClass", "Draenic Stone") <<
		QApplication::translate("gicClass", "Skull of War") <<
		QApplication::translate("gicClass", "Mote of the Mountain") <<
		QApplication::translate("gicClass", "Worldbreaker's Resolve") <<
		QApplication::translate("gicClass", "Discordant Chorus") << 
		QApplication::translate("gicClass", "Empty Drinking Horn") << 
		QApplication::translate("gicClass", "Unending Hunger") <<
		QApplication::translate("gicClass", "Spores of Alacrity") <<
		QApplication::translate("gicClass", "Bonemaw's Big Toe") <<
		QApplication::translate("gicClass", "Emberscale Talisman");
	ui.comboTrinketSpecial1->addItems(lists);
	ui.comboTrinketSpecial2->addItems(lists);
	lists.clear();

	lists <<
		QApplication::translate("gicClass", "Not set.") <<
		QApplication::translate("gicClass", "Human") <<
		QApplication::translate("gicClass", "Dwarf") <<
		QApplication::translate("gicClass", "Gnome") <<
		QApplication::translate("gicClass", "Night Elf(day)") <<
		QApplication::translate("gicClass", "Night Elf(night)") <<
		QApplication::translate("gicClass", "Draenei") <<
		QApplication::translate("gicClass", "Worgen") <<
		QApplication::translate("gicClass", "Orc") <<
		QApplication::translate("gicClass", "Troll") <<
		QApplication::translate("gicClass", "Tauren") <<
		QApplication::translate("gicClass", "Undead") <<
		QApplication::translate("gicClass", "Blood Elf") <<
		QApplication::translate("gicClass", "Goblin") <<
		QApplication::translate("gicClass", "Pandaren");
	ui.comboRace->addItems(lists);
	lists.clear();

	ui.comboTalent1->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent1->addItem(QApplication::translate("gicClass", "Juggernaut"));
	ui.comboTalent1->addItem(QApplication::translate("gicClass", "Double Time"));
	ui.comboTalent1->addItem(QApplication::translate("gicClass", "Warbringer"));

	ui.comboTalent2->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent2->addItem(QApplication::translate("gicClass", "Enraged Regeneration"));
	ui.comboTalent2->addItem(QApplication::translate("gicClass", "Second Wind"));
	ui.comboTalent2->addItem(QApplication::translate("gicClass", "Impending Victory"));

	ui.comboTalent3->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent3->addItem(QApplication::translate("gicClass", "Furious Strike"));
	ui.comboTalent3->addItem(QApplication::translate("gicClass", "Sudden Death"));
	ui.comboTalent3->addItem(QApplication::translate("gicClass", "Unquenchable Thirst"));

	ui.comboTalent4->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent4->addItem(QApplication::translate("gicClass", "Storm Bolt"));
	ui.comboTalent4->addItem(QApplication::translate("gicClass", "Shockwave"));
	ui.comboTalent4->addItem(QApplication::translate("gicClass", "Dragon Roar"));

	ui.comboTalent5->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent5->addItem(QApplication::translate("gicClass", "Mass Spell Reflection"));
	ui.comboTalent5->addItem(QApplication::translate("gicClass", "Safeguard"));
	ui.comboTalent5->addItem(QApplication::translate("gicClass", "Vigilance"));

	ui.comboTalent6->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent6->addItem(QApplication::translate("gicClass", "Avatar"));
	ui.comboTalent6->addItem(QApplication::translate("gicClass", "Bloodbath"));
	ui.comboTalent6->addItem(QApplication::translate("gicClass", "Bladestorm"));

	ui.comboTalent7->addItem(QApplication::translate("gicClass", "Not set."));
	ui.comboTalent7->addItem(QApplication::translate("gicClass", "Anger Management"));
	ui.comboTalent7->addItem(QApplication::translate("gicClass", "Ravager"));
	ui.comboTalent7->addItem(QApplication::translate("gicClass", "Siegebreaker"));

	ui.tableGearSummary->setItem(1, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Strength")));
	ui.tableGearSummary->setItem(2, 0, new QTableWidgetItem(QApplication::translate("gicClass", "AP")));
	ui.tableGearSummary->setItem(3, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Crit")));
	ui.tableGearSummary->setItem(4, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Haste")));
	ui.tableGearSummary->setItem(5, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Mastery")));
	ui.tableGearSummary->setItem(6, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Mult")));
	ui.tableGearSummary->setItem(7, 0, new QTableWidgetItem(QApplication::translate("gicClass", "Vers")));
	ui.tableGearSummary->setItem(0, 1, new QTableWidgetItem(QApplication::translate("gicClass", "Gear")));
	ui.tableGearSummary->setItem(0, 2, new QTableWidgetItem(QApplication::translate("gicClass", "Buffed")));

	QString* p = gear_type_list[selected_gear_slot];
	while (p->length())
		ui.comboItemType->addItem(*p++);
	ui.comboItemType->setCurrentIndex(gear_list[selected_gear_slot].type);
	gear_summary_calculate();

}

gic::~gic()
{

}

void gic::TxtBoxNotify(QString value) {
	ui.txtResult->moveCursor(QTextCursor::End);
	ui.txtResult->insertPlainText(value);
}

class functionbuf
	: public std::streambuf {
private:
	typedef std::streambuf::traits_type traits_type;
	char          d_buffer[102400];
	gic* pg;
	int overflow(int c) {
		if (!traits_type::eq_int_type(c, traits_type::eof())) {
			*this->pptr() = traits_type::to_char_type(c);
			this->pbump(1);
		}
		return this->sync() ? traits_type::not_eof(c) : traits_type::eof();
	}
	int sync() {
		if (this->pbase() != this->pptr()) {
			QMetaObject::invokeMethod(pg, "TxtBoxNotify", Q_ARG(QString, QString(std::string(this->pbase(), this->pptr()).c_str())));
			this->setp(this->pbase(), this->epptr());
		}
		return 0;
	}
public:
	functionbuf(gic* pg) : pg(pg)
		 {
		this->setp(this->d_buffer, this->d_buffer + sizeof(this->d_buffer) - 1);
	}
};

class ofunctionstream
	: private virtual functionbuf
	, public std::ostream {
public:
	ofunctionstream(gic* pg)
		: functionbuf(pg), std::ostream(static_cast<std::streambuf*>(this)) {
		this->flags(std::ios_base::unitbuf);
	}
};

void gic::on_btnRun_clicked()
{
	reset_result_page(); 
	ui.tabWidget->setCurrentWidget(ui.tabResult);
	ui.btnRun->setDisabled(true);
	set_default_parameters();
	gear_summary_calculate();
	QtConcurrent::run(this, &gic::run_simulation);
}

ofunctionstream* simlog = NULL;

void gic::set_arguments(){
	calculate_scale_factors = ui.checkCalculateScaleFactors->isChecked() ? 1 : 0;
	iterations = ui.comboIterations->currentData().toInt();
	rng_engine = ui.comboRNG->currentData().toInt();
	seed = ui.checkDeterministic->isChecked() ? 4262 : 0;
	ocl().opencl_device_id = ui.comboDevices->currentIndex();
	strict_gcd = ui.checkStrictGCD->isChecked();
	sync_melee = ui.checkSyncMelee->isChecked();
	wbr_never_expire = ui.checkWBRNeverExpire->isChecked();
	avatar_like_bloodbath = ui.checkAvatarLikeBloodbath->isChecked();

	max_length = ui.comboCombatLength->currentData().toInt();
	vary_combat_length = ui.comboVaryCombatLength->currentData().toFloat();
	initial_health_percentage = ui.comboInitialHealthPercentage->currentData().toFloat();
	death_pct = ui.comboDeathPct->currentData().toFloat();
	raidbuff.ap = ui.checkRaidBuffAP->isChecked();
	raidbuff.bloodlust = ui.checkRaidBuffBloodlust->isChecked();
	raidbuff.crit = ui.checkRaidBuffCrit->isChecked();
	raidbuff.flask = ui.checkRaidBuffFlask->isChecked();
	raidbuff.food = ui.checkRaidBuffFood->isChecked();
	raidbuff.haste = ui.checkRaidBuffHaste->isChecked();
	raidbuff.mastery = ui.checkRaidBuffMastery->isChecked();
	raidbuff.mult = ui.checkRaidBuffMult->isChecked();
	raidbuff.potion = ui.checkRaidBuffPotion->isChecked();
	raidbuff.sp = ui.checkRaidBuffSP->isChecked();
	raidbuff.sta = ui.checkRaidBuffSta->isChecked();
	raidbuff.str = ui.checkRaidBuffStr->isChecked();
	raidbuff.vers = ui.checkRaidBuffVers->isChecked();

	talent = 0;
	talent += ui.comboTalent7->currentIndex();
	talent += ui.comboTalent6->currentIndex() * 10;
	talent += ui.comboTalent5->currentIndex() * 100;
	talent += ui.comboTalent4->currentIndex() * 1000;
	talent += ui.comboTalent3->currentIndex() * 10000;
	talent += ui.comboTalent2->currentIndex() * 100000;
	talent += ui.comboTalent1->currentIndex() * 1000000;
	race = ui.comboRace->currentIndex();
	trinket1 = ui.comboTrinketSpecial1->currentIndex();
	trinket2 = ui.comboTrinketSpecial2->currentIndex();
	trinket1_ilvl = ui.txtTrinketValue1->text().toInt();
	trinket2_ilvl = ui.txtTrinketValue2->text().toInt();

	mh_high = ui.txtMHHigh->text().toInt();
	oh_high = ui.txtOHHigh->text().toInt();
	mh_low = ui.txtMHLow->text().toInt();
	oh_low = ui.txtOHLow->text().toInt();
	mh_speed = ui.txtMHSpeed->text().toFloat();
	oh_speed = ui.txtOHSpeed->text().toFloat();
	mh_type = gear_list[6].type;
	oh_type = gear_list[7].type;

	power_max = 100;
	if (ui.checkGlyphOfUnendingRage->isChecked()) power_max += 20;
	if (race == 3) power_max *= 1.05;
	t17_2pc = ui.checkT172P->isChecked();
	t17_4pc = ui.checkT174P->isChecked();
	t18_2pc = ui.checkT182P->isChecked();
	t18_4pc = ui.checkT184P->isChecked();
	archmages_incandescence = (ui.comboIncandescence->currentIndex() == 1);
	archmages_greater_incandescence = (ui.comboIncandescence->currentIndex() == 2);
	if (ui.comboIncandescence->currentIndex() == 3){
		legendary_ring = ui.txtLegendaryRing->text().toInt();
		legendary_ring = 2500.0 * pow(ilvlScaleCoeff, legendary_ring - 735);
	}
	else legendary_ring = 0;
	thunderlord_mh = (ui.comboMHEnchant->currentIndex() == 1);
	bleeding_hollow_mh = (ui.comboMHEnchant->currentIndex() == 2);
	shattered_hand_mh = (ui.comboMHEnchant->currentIndex() == 3);
	thunderlord_oh = (ui.comboOHEnchant->currentIndex() == 1);
	bleeding_hollow_oh = (ui.comboOHEnchant->currentIndex() == 2);
	shattered_hand_oh = (ui.comboOHEnchant->currentIndex() == 3);
}

void gic::run_simulation(){
	char header[80];
	time_t rawtime;
	time(&rawtime);
	struct tm* ts = localtime(&rawtime);
	strftime(header, 80, "============================== %H:%M:%S ==============================", ts);

	stat_array.clear();

	if (!simlog) simlog = new ofunctionstream(this);
	report_path = simlog;
	*report_path << header << std::endl;
	*report_path << QApplication::translate("gicClass", "Set arguments...\n").toStdString();
	set_arguments();
	apl = ui.txtAPL->toPlainText().toStdString();
	default_actions = ui.checkDefaultActions->isChecked();
	
	if (ui.txtOverride->toPlainText().length()){
		std::vector<kvpair_t> arglist;
		QString f = ui.txtOverride->toPlainText();
		char* cf = (char*)calloc(f.toStdString().length()+1,1);
		strcpy(cf, f.toStdString().c_str());
		{
			int i = 0;
			char ** rargv = (char**)calloc(65536, sizeof(char*));
			int rargc = 1;
			char buffer[4096];
			size_t buffer_i = 0;
			char ch;
			int comment = 0;
			do {
				ch = cf[i++];
				if (!comment){
					if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF){
						if (buffer_i){
							buffer[buffer_i++] = 0;
							rargv[rargc] = (char*)malloc(buffer_i);
							memcpy(rargv[rargc], buffer, buffer_i);
							rargc++;
							buffer_i = 0;
							if (rargc >= 65536) { *report_path << "Configuration file too long." << std::endl; }
						}
					}
					else if (ch == '#' && buffer_i == 0){
						comment = 1;
					}
					else{
						buffer[buffer_i++] = ch;
						if (buffer_i >= 4096) { *report_path << "Configuration line too long." << std::endl; }
					}
				}
				else{
					if (ch == '\r' || ch == '\n'){
						comment = 0;
						buffer_i = 0;
					}
				}
			} while (ch != EOF);
			build_arglist(arglist, rargc, rargv);
			for (int i = 1; i < rargc; i++){
				free(rargv[i]);
			}
			free(rargv);
		}
		parse_parameters(arglist);
	}
	parameters_consistency();
	generate_predef();
	
	if (developer_debug) *report_path << predef;

	*report_path << QApplication::translate("gicClass", "Initializing compute cnvironment...\n").toStdString();
	
	ocl().init();
	
	// Start simulation.
	*report_path << QApplication::translate("gicClass", "Start simulation...\n").toStdString();
	ocl().run(apl, predef);

	*report_path << QApplication::translate("gicClass", "Simulation finished.\n").toStdString();



	if (calculate_scale_factors){
		float weight[6];
		float weighterror[6];
		const char* weightname[6];

		float unitweight = 0;
		*report_path << "Scale factors:" << std::endl;
		const char* stat_name[] = {
			0, "str", "crit", "haste", "mastery", "mult", "vers",
		};
		float sf, sfe;
		for (int i = 1; i < stat_array.size(); i++){
			sf = stat_array[i].dps - stat_array[0].dps;
			sf /= 120.0;
			sfe = stat_array[i].dpse * stat_array[i].dpse + stat_array[0].dpse * stat_array[0].dpse;
			sfe = sqrt(sfe) / 120.0;
			*report_path << stat_name[i] << " " << sf << ", error " << sfe << std::endl;
			weight[i-1] = sf;
			weighterror[i-1] = sfe;
			weightname[i - 1] = stat_name[i];
			unitweight = unitweight >(sf + sfe) ? unitweight : (sf + sfe);
		}
		
		for (int i = 0; i < 6; i++){
			weight[i] = weight[i] / unitweight;
			weighterror[i] = weighterror[i] / unitweight;
		}

		int converge = 1;
		do{
			converge = 1;
			for (int i = 0; i < 5; i++){
				if (weight[i] < weight[i + 1]){
					converge = 0;
					auto tw = weight[i];
					auto twe = weighterror[i];
					auto twn = weightname[i];
					weight[i] = weight[i + 1];
					weighterror[i] = weighterror[i + 1];
					weightname[i] = weightname[i + 1];
					weight[i + 1] = tw;
					weighterror[i + 1] = twe;
					weightname[i + 1] = twn;
				}
			}
		} while (!converge);

		ui.lblWeight->setText(weightname[0]);
		ui.barWeight->setGeometry(QRect(56, 0, 175 * abs(weight[0] - weighterror[0]), 19));
		ui.barError->setGeometry(QRect(56 + 175 * abs(weight[0] - weighterror[0]), 0, 175 * weighterror[0] * 2, 19));

		ui.lblWeight_2->setText(weightname[1]);
		ui.barWeight_2->setGeometry(QRect(56, 0, 175 * abs(weight[1] - weighterror[1]), 19));
		ui.barError_2->setGeometry(QRect(56 + 175 * abs(weight[1] - weighterror[1]), 0, 175 * weighterror[1] * 2, 19));

		ui.lblWeight_3->setText(weightname[2]);
		ui.barWeight_3->setGeometry(QRect(56, 0, 175 * abs(weight[2] - weighterror[2]), 19));
		ui.barError_3->setGeometry(QRect(56 + 175 * abs(weight[2] - weighterror[2]), 0, 175 * weighterror[2] * 2, 19));

		ui.lblWeight_4->setText(weightname[3]);
		ui.barWeight_4->setGeometry(QRect(56, 0, 175 * abs(weight[3] - weighterror[3]), 19));
		ui.barError_4->setGeometry(QRect(56 + 175 * abs(weight[3] - weighterror[3]), 0, 175 * weighterror[3] * 2, 19));

		ui.lblWeight_5->setText(weightname[4]);
		ui.barWeight_5->setGeometry(QRect(56, 0, 175 * abs(weight[4] - weighterror[4]), 19));
		ui.barError_5->setGeometry(QRect(56 + 175 * abs(weight[4] - weighterror[4]), 0, 175 * weighterror[4] * 2, 19));

		ui.lblWeight_6->setText(weightname[5]);
		ui.barWeight_6->setGeometry(QRect(56, 0, 175 * abs(weight[5] - weighterror[5]), 19));
		ui.barError_6->setGeometry(QRect(56 + 175 * abs(weight[5] - weighterror[5]), 0, 175 * weighterror[5] * 2, 19));

	}
	calculate_scale_factors = 0;

	char temp[32];
	sprintf(temp, "%.1f", stat_array[0].dps);
	ui.lblDPS->setText(temp);
	sprintf(temp, "%.1f", stat_array[0].dpsr);
	ui.lblDPSRange->setText(temp);
	sprintf(temp, "%.3f", stat_array[0].dpse);
	ui.lblDPSError->setText(temp);

	stat_array.clear();
	current_stat.gear_str = 0;
	current_stat.gear_crit = 0;
	current_stat.gear_haste = 0;
	current_stat.gear_mastery = 0;
	current_stat.gear_mult = 0;
	current_stat.gear_vers = 0;
	trinket1 = ui.comboTrinketSpecial1->currentIndex();
	trinket2 = ui.comboTrinketSpecial2->currentIndex();
	trinket1_ilvl = ui.txtTrinketValue1->text().toInt();
	trinket2_ilvl = ui.txtTrinketValue2->text().toInt();
	for (int i = 0; i < 16; i++){
		plate_specialization = plate_specialization && ((gear_type_list[i] != gear_type_list_armor) || (gear_list[i].type == 0));
		current_stat.gear_str += gear_list[i].str;
		current_stat.gear_crit += gear_list[i].crit;
		current_stat.gear_haste += gear_list[i].haste;
		current_stat.gear_mastery += gear_list[i].mastery;
		current_stat.gear_mult += gear_list[i].mult;
		current_stat.gear_vers += gear_list[i].vers;
	}
	stat_array.push_back(current_stat);
	parameters_consistency();
	generate_predef();

	if (ui.checkCalculateMaxima->isChecked()){
		*report_path << QApplication::translate("gicClass", "Maxima Descent Start.\n").toStdString();
		int init_interval = ui.txtDescentInitInterval->text().toInt();
		int min_interval = ui.txtDescentMinInterval->text().toInt();
		int max_iteration_limit = ui.txtDescentIterationLimit->text().toInt();
		if (!init_interval) init_interval = 320;
		if (!min_interval) min_interval = 40;
		if (!max_iteration_limit) max_iteration_limit = 800000;
		descent(init_interval,min_interval, max_iteration_limit);
		*report_path << QApplication::translate("gicClass", "Maxima Descent Finished.\n").toStdString();
	}

	stat_array.clear();
	current_stat.gear_str = 0;
	current_stat.gear_crit = 0;
	current_stat.gear_haste = 0;
	current_stat.gear_mastery = 0;
	current_stat.gear_mult = 0;
	current_stat.gear_vers = 0;
	trinket1 = ui.comboTrinketSpecial1->currentIndex();
	trinket2 = ui.comboTrinketSpecial2->currentIndex();
	trinket1_ilvl = ui.txtTrinketValue1->text().toInt();
	trinket2_ilvl = ui.txtTrinketValue2->text().toInt();
	for (int i = 0; i < 16; i++){
		plate_specialization = plate_specialization && ((gear_type_list[i] != gear_type_list_armor) || (gear_list[i].type == 0));
		current_stat.gear_str += gear_list[i].str;
		current_stat.gear_crit += gear_list[i].crit;
		current_stat.gear_haste += gear_list[i].haste;
		current_stat.gear_mastery += gear_list[i].mastery;
		current_stat.gear_mult += gear_list[i].mult;
		current_stat.gear_vers += gear_list[i].vers;
	}
	stat_array.push_back(current_stat);
	parameters_consistency();
	generate_predef();

	if (ui.checkCalculatePlot->isChecked()){
		*report_path << QApplication::translate("gicClass", "Contour Plot Start.\n").toStdString();
		int interval = ui.txtPlotInterval->text().toInt();
		double error_tolerance = ui.txtPlotErrorTolerance->text().toDouble();
		int max_iteration_limit = ui.txtPlotMaxIterationLimit->text().toInt();
		if (!interval) interval = 50;
		if (!error_tolerance) error_tolerance = 5.0;
		if (!max_iteration_limit) max_iteration_limit = 400000;
		unsigned mask = 0, checkmask, correct = 1;
		if (ui.checkPlotCrit->isChecked()) mask |= 1;
		if (ui.checkPlotHaste->isChecked()) mask |= 2;
		if (ui.checkPlotMastery->isChecked()) mask |= 4;
		if (ui.checkPlotMult->isChecked()) mask |= 8;
		if (ui.checkPlotVers->isChecked()) mask |= 16;
		checkmask = mask;
		if (!checkmask) correct = 0;
		checkmask &= checkmask - 1;
		if (!checkmask) correct = 0;
		checkmask &= checkmask - 1;
		if (!checkmask) correct = 0;
		checkmask &= checkmask - 1;
		if (checkmask) correct = 0;
		if (!correct){
			*report_path << QApplication::translate("gicClass", "Contour Plot stats not set properly. Exact 3 stats should be checked.\nReset to Crit-Haste-Mastery.\n").toStdString();
			mask = 7;
		}
		plot(mask, interval, error_tolerance, max_iteration_limit);
		*report_path << QApplication::translate("gicClass", "Contour Plot Finished. Run \"plot.sci\" with SciLab(http://scilab.org/) to draw a figure.\n").toStdString();
	}

	if (ui.checkTrinketBenchmark->isChecked())
		trinket_benchmark();

	report_path->flush();
	ocl().free();
	QMetaObject::invokeMethod(ui.btnRun, "setDisabled", Q_ARG(bool, false));
}


void gic::on_listActions_itemDoubleClicked()
{ 
	if (ui.listActions->currentItem())
		ui.txtAPL->textCursor().insertText(ui.listActions->currentItem()->text());
}

void gic::on_listConditions_itemDoubleClicked()
{
	if (ui.listConditions->currentItem())
		ui.txtAPL->textCursor().insertText(ui.listConditions->currentItem()->text());
}

void gic::on_comboIncandescence_currentIndexChanged(int idx){
	ui.txtLegendaryRing->setEnabled(ui.comboIncandescence->currentIndex() == 3);
}

void gic::on_btnImport_clicked()
{
	ui.btnImport->setDisabled(true);
	std::string region;
	std::string realm;
	std::string name;
	region = ui.comboRegion->currentText().toStdString();
	realm = ui.txtRealm->text().toStdString();
	name = ui.txtCharacter->text().toStdString();
	import_player(realm, name, region);
	ui.btnImport->setDisabled(false);
}

void gic::on_btnGenerateDefaultAPL_clicked()
{
	set_arguments();
	auto_apl();
	ui.txtAPL->setPlainText(QString(apl.c_str()));
}

void gic::mh_dps_calculate()
{
	mh_high = ui.txtMHHigh->text().toInt();
	mh_low = ui.txtMHLow->text().toInt();
	mh_speed = ui.txtMHSpeed->text().toFloat();
	if (mh_speed <= .0) mh_speed = 1.5;
	float mh_dps = (mh_high + mh_low) * 0.5 / mh_speed;
	char buf[32];
	sprintf(buf, "%.1f", mh_dps + 0.05);
	ui.lblMHDPS->setText(buf);
}
void gic::oh_dps_calculate()
{
	oh_high = ui.txtOHHigh->text().toInt();
	oh_low = ui.txtOHLow->text().toInt();
	oh_speed = ui.txtOHSpeed->text().toFloat();
	if (oh_speed <= .0) oh_speed = 1.5;
	float oh_dps = (oh_high + oh_low) * 0.5 / oh_speed;
	char buf[32];
	sprintf(buf, "%.1f", oh_dps + 0.05);
	ui.lblOHDPS->setText(buf);
}

QString qsprint(int v){
	char buf[32];
	sprintf(buf, "%d", v);
	return buf;
}
QString qsprint(float v){
	char buf[32];
	sprintf(buf, "%.2f%%", v * 100.f);
	return buf;
}

void gic::select_gear_slot()
{
	if (ui.radioHelm->isChecked()) selected_gear_slot = 0;
	if (ui.radioNeck->isChecked()) selected_gear_slot = 1;
	if (ui.radioShoulder->isChecked()) selected_gear_slot = 2;
	if (ui.radioBack->isChecked()) selected_gear_slot = 3;
	if (ui.radioChest->isChecked()) selected_gear_slot = 4;
	if (ui.radioWrist->isChecked()) selected_gear_slot = 5;
	if (ui.radioMainhand->isChecked()) selected_gear_slot = 6;
	if (ui.radioOffhand->isChecked()) selected_gear_slot = 7;
	if (ui.radioHand->isChecked()) selected_gear_slot = 8;
	if (ui.radioWaist->isChecked()) selected_gear_slot = 9;
	if (ui.radioLeg->isChecked()) selected_gear_slot = 10;
	if (ui.radioFeet->isChecked()) selected_gear_slot = 11;
	if (ui.radioFinger1->isChecked()) selected_gear_slot = 12;
	if (ui.radioFinger2->isChecked()) selected_gear_slot = 13;
	if (ui.radioTrinket1->isChecked()) selected_gear_slot = 14;
	if (ui.radioTrinket2->isChecked()) selected_gear_slot = 15;
	ui.lblItemName->setText(QString(gear_list[selected_gear_slot].name.c_str()));
	ui.txtItemStr->setText(qsprint(gear_list[selected_gear_slot].str));
	ui.txtItemCrit->setText(qsprint(gear_list[selected_gear_slot].crit));
	ui.txtItemHaste->setText(qsprint(gear_list[selected_gear_slot].haste));
	ui.txtItemMastery->setText(qsprint(gear_list[selected_gear_slot].mastery));
	ui.txtItemMult->setText(qsprint(gear_list[selected_gear_slot].mult));
	ui.txtItemVers->setText(qsprint(gear_list[selected_gear_slot].vers));
	ui.comboItemType->clear();
	QString* p = gear_type_list[selected_gear_slot];
	while (p->length())
		ui.comboItemType->addItem(*p++);
	ui.comboItemType->setCurrentIndex(gear_list[selected_gear_slot].type);
}

#define RACE_NONE 0
#define RACE_HUMAN 1
#define RACE_DWARF 2
#define RACE_GNOME 3
#define RACE_NIGHTELF_DAY 4
#define RACE_NIGHTELF_NIGHT 5
#define RACE_DRAENEI 6
#define RACE_WORGEN 7
#define RACE_ORC 8
#define RACE_TROLL 9
#define RACE_TAUREN 10
#define RACE_UNDEAD 11
#define RACE_BLOODELF 12
#define RACE_GOBLIN 13
#define RACE_PANDAREN 14

void gic::gear_summary_calculate()
{
	race = ui.comboRace->currentIndex();
	gear_list[selected_gear_slot].type = ui.comboItemType->currentIndex();
	gear_list[selected_gear_slot].str = ui.txtItemStr->text().toInt();
	gear_list[selected_gear_slot].crit = ui.txtItemCrit->text().toInt();
	gear_list[selected_gear_slot].haste = ui.txtItemHaste->text().toInt();
	gear_list[selected_gear_slot].mastery = ui.txtItemMastery->text().toInt();
	gear_list[selected_gear_slot].mult = ui.txtItemMult->text().toInt();
	gear_list[selected_gear_slot].vers = ui.txtItemVers->text().toInt();

	plate_specialization = 1;
	int str = 0, crit = 0, haste = 0, mastery = 0, mult = 0, vers = 0, ap = 0;

	for (int i = 0; i < 16; i++){
		plate_specialization = plate_specialization && ((gear_type_list[i] != gear_type_list_armor) || (gear_list[i].type == 0));
		str += gear_list[i].str;
		crit += gear_list[i].crit;
		haste += gear_list[i].haste;
		mastery += gear_list[i].mastery;
		mult += gear_list[i].mult;
		vers += gear_list[i].vers;
	}

	ui.tableGearSummary->setItem(1, 1, new QTableWidgetItem(qsprint(str)));
	ui.tableGearSummary->setItem(3, 1, new QTableWidgetItem(qsprint(crit)));
	ui.tableGearSummary->setItem(4, 1, new QTableWidgetItem(qsprint(haste)));
	ui.tableGearSummary->setItem(5, 1, new QTableWidgetItem(qsprint(mastery)));
	ui.tableGearSummary->setItem(6, 1, new QTableWidgetItem(qsprint(mult)));
	ui.tableGearSummary->setItem(7, 1, new QTableWidgetItem(qsprint(vers)));
	
	current_stat.gear_str = str;
	current_stat.gear_crit = crit;
	current_stat.gear_mastery = mastery;
	current_stat.gear_haste = haste;
	current_stat.gear_mult = mult;
	current_stat.gear_vers = vers;

	if (ui.checkRaidBuffFlask->isChecked()) str += 250;
	if (ui.checkRaidBuffFood->isChecked()) crit += 125 * (ui.comboRace->currentIndex() == 14 ? 2 : 1);



	int racial_base_str[] = {
		0, 0, 5, -5, -4, -4, 66, 3, 3, 1, 5, -1, -3, -3, 0,
	};

	float fstr = str;
	float coeff = 1.0f;
	if (plate_specialization) coeff *= 1.05f;
	if (ui.checkRaidBuffStr->isChecked()) coeff *= 1.05f;
	str = (int)(fstr * coeff);
	fstr = 1455; /* Base str @lvl 100. */
	fstr += racial_base_str[race]; /* Racial str. */
	str += (int)(fstr * coeff);
	
	ap = str;
	if (ui.checkRaidBuffAP->isChecked()) ap = (int)(ap * 1.1f + 0.5f);

	float fmastery = (float)mastery;
	if (ui.checkRaidBuffMastery->isChecked()) fmastery += 550;
	fmastery = 1.4f * (0.08f + fmastery / 11000);

	float fcrit = (float)crit;
	fcrit *= 1.05f;
	fcrit = 0.05f + fcrit / 11000;
	if (ui.checkRaidBuffCrit->isChecked()) fcrit += 0.05f;
	if ((race == RACE_NIGHTELF_DAY) || (race == RACE_BLOODELF) || (race == RACE_WORGEN))
		fcrit += 0.01f;

	float fhaste = (float)haste;
	fhaste = 1.0f + fhaste / 9000;
	if (ui.checkRaidBuffHaste->isChecked()) fhaste *= 1.05f;
	if ((race == RACE_NIGHTELF_NIGHT) || (race == RACE_GOBLIN) || (race == RACE_GNOME))
		fhaste *= 1.01f;
	fhaste = fhaste - 1.0f;

	float fmult = (float)mult;
	fmult = fmult / 6600;
	if (ui.checkRaidBuffMult->isChecked()) fmult += 0.05f;

	float fvers = (float)vers;
	if (race == RACE_HUMAN) fvers += 100;
	fvers = fvers / 13000;
	if (ui.checkRaidBuffVers->isChecked()) fvers += 0.03f;

	ui.tableGearSummary->setItem(1, 2, new QTableWidgetItem(qsprint(str)));
	ui.tableGearSummary->setItem(2, 2, new QTableWidgetItem(qsprint(ap)));
	ui.tableGearSummary->setItem(3, 2, new QTableWidgetItem(qsprint(fcrit)));
	ui.tableGearSummary->setItem(4, 2, new QTableWidgetItem(qsprint(fhaste)));
	ui.tableGearSummary->setItem(5, 2, new QTableWidgetItem(qsprint(fmastery)));
	ui.tableGearSummary->setItem(6, 2, new QTableWidgetItem(qsprint(fmult)));
	ui.tableGearSummary->setItem(7, 2, new QTableWidgetItem(qsprint(fvers)));
}

void gic::on_btnResetBuild_clicked(){
	ui.txtCharacter->clear();
	ui.txtRealm->clear();
	ui.txtItemCrit->clear();
	ui.txtItemHaste->clear();
	ui.txtItemMastery->clear();
	ui.txtItemMult->clear();
	ui.txtItemStr->clear();
	ui.txtItemVers->clear();
	ui.txtLegendaryRing->clear();
	ui.txtMHHigh->clear();
	ui.txtMHLow->clear();
	ui.txtMHSpeed->clear();
	ui.txtOHHigh->clear();
	ui.txtOHLow->clear();
	ui.txtOHSpeed->clear();
	ui.txtTrinketValue1->clear();
	ui.txtTrinketValue2->clear();
	ui.comboIncandescence->setCurrentIndex(0);
	ui.comboItemType->setCurrentIndex(0);
	ui.comboMHEnchant->setCurrentIndex(0);
	ui.comboOHEnchant->setCurrentIndex(0);
	ui.comboRace->setCurrentIndex(0);
	ui.comboRegion->setCurrentIndex(0);
	ui.comboTalent1->setCurrentIndex(0);
	ui.comboTalent2->setCurrentIndex(0);
	ui.comboTalent3->setCurrentIndex(0);
	ui.comboTalent4->setCurrentIndex(0);
	ui.comboTalent5->setCurrentIndex(0);
	ui.comboTalent6->setCurrentIndex(0);
	ui.comboTalent7->setCurrentIndex(0);
	ui.comboTrinketSpecial1->setCurrentIndex(0);
	ui.comboTrinketSpecial2->setCurrentIndex(0);
	ui.checkGlyphOfUnendingRage->setChecked(false);
	ui.checkT172P->setChecked(false);
	ui.checkT174P->setChecked(false);
	ui.checkT182P->setChecked(false);
	ui.checkT184P->setChecked(false);
	for (int i = 0; i < 16; i++){
		gear_list[i] = item_t();
	}
	set_arguments();
	ui.radioHelm->setChecked(true);
}