#include "gic.h"
#include "VersionNo.h"
#include "irecore.h"

void set_default_parameters();

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
	reset_result_page();

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

}

gic::~gic()
{

}


extern std::string apl;
extern std::string predef;
void parameters_consistency();
void generate_predef();


class functionbuf
	: public std::streambuf {
private:
	typedef std::streambuf::traits_type traits_type;
	std::function<void(std::string)> d_function;
	char          d_buffer[102400];
	int overflow(int c) {
		if (!traits_type::eq_int_type(c, traits_type::eof())) {
			*this->pptr() = traits_type::to_char_type(c);
			this->pbump(1);
		}
		return this->sync() ? traits_type::not_eof(c) : traits_type::eof();
	}
	int sync() {
		if (this->pbase() != this->pptr()) {
			this->d_function(std::string(this->pbase(), this->pptr()));
			this->setp(this->pbase(), this->epptr());
		}
		return 0;
	}
public:
	functionbuf(std::function<void(std::string)> const& function)
		: d_function(function) {
		this->setp(this->d_buffer, this->d_buffer + sizeof(this->d_buffer) - 1);
	}
};

class ofunctionstream
	: private virtual functionbuf
	, public std::ostream {
public:
	ofunctionstream(std::function<void(std::string)> const& function)
		: functionbuf(function)
		, std::ostream(static_cast<std::streambuf*>(this)) {
		//this->flags(std::ios_base::unitbuf);
	}
};

QTextBrowser* TB;

void TxtBoxNotify(std::string const& value) {
	TB->append(value.c_str());
}


void gic::on_btnRun_clicked()
{
	char header[80];
	time_t rawtime;
	time(&rawtime);
	struct tm* ts = localtime(&rawtime);
	
	strftime(header, 80, "============================== %H:%M:%S ==============================", ts);
	ui.txtResult->append(header);

	reset_result_page();
	set_default_parameters();
	stat_array.clear();

	ofunctionstream simlog(&TxtBoxNotify);
	TB = ui.txtResult;
	report_path = &simlog;
	ui.txtResult->append(QApplication::translate("gicClass", "Set arguments..."));
	// TODO: set arguments;
	calculate_scale_factors = ui.checkCalculateScaleFactors->isChecked() ? 1 : 0;
	iterations = ui.comboIterations->currentData().toInt();
	rng_engine = ui.comboRNG->currentData().toInt();
	seed = ui.checkDeterministic->isChecked() ? 4262 : 0;
	ocl().opencl_device_id = ui.comboDevices->currentIndex();
	
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


	ui.txtResult->append(QApplication::translate("gicClass", "Initializing compute cnvironment..."));
	parameters_consistency();
	generate_predef();

	ocl().init();
	
	// Start simulation.
	ui.txtResult->append(QApplication::translate("gicClass", "Start simulation..."));
	ocl().run(apl, predef);

	ui.txtResult->append(QApplication::translate("gicClass", "Simulation finished."));



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

	char temp[32];
	sprintf(temp, "%.1f", stat_array[0].dps);
	ui.lblDPS->setText(temp);
	sprintf(temp, "%.1f", stat_array[0].dpsr);
	ui.lblDPSRange->setText(temp);
	sprintf(temp, "%.3f", stat_array[0].dpse);
	ui.lblDPSError->setText(temp);


	report_path->flush();
	ocl().free();
}

