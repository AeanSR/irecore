#include "irecore.h"

stat_t stat;
std::vector<stat_t> stat_array;
int stat_not_pushed = 1;
raidbuff_t raidbuff;
cl_uint seed;
std::string apl;
std::string predef;
int iterations;
float vary_combat_length;
float max_length;
float initial_health_percentage;
float death_pct;
float power_max; // rage_max
int plate_specialization;
int single_minded;
int race;
float mh_speed;
float oh_speed;
int mh_low, mh_high;
int oh_low, oh_high;
int mh_type;
int oh_type;
int talent;
int rng_engine;
const int base10[] = { 1000000, 100000, 10000, 1000, 100, 10, 1 };
#define TALENT_TIER(tier) ((talent / base10[tier - 1]) % 10)

int archmages_incandescence;
int archmages_greater_incandescence;
int t17_2pc;
int t17_4pc;
int thunderlord_mh;
int thunderlord_oh;
int bleeding_hollow_mh;
int bleeding_hollow_oh;
int shattered_hand_mh;
int shattered_hand_oh;

int developer_debug;
int list_available_devices;

const char* race_str_kernel[] = {
	"RACE_NONE",
	"RACE_HUMAN",
	"RACE_DWARF",
	"RACE_GNOME",
	"RACE_NIGHTELF_DAY",
	"RACE_NIGHTELF_NIGHT",
	"RACE_DRAENEI",
	"RACE_WORGEN",
	"RACE_ORC",
	"RACE_TROLL",
	"RACE_TAUREN",
	"RACE_UNDEAD",
	"RACE_BLOODELF",
	"RACE_GOBLIN",
	"RACE_PANDAREN",
	NULL
};
const char* race_str_param[] = {
	"none",
	"human",
	"dwarf",
	"gnome",
	"nightelf_day",
	"nightelf_night",
	"draenei",
	"worgen",
	"orc",
	"troll",
	"tauren",
	"undead",
	"bloodelf",
	"goblin",
	"pandaren",
	NULL
};
const char* weapon_type_str[] = {
	"WEAPON_2H",
	"WEAPON_1H",
	"WEAPON_DAGGER",
};

void err(const char* format, ...){
	va_list vl;
	va_start(vl, format);
	vfprintf(stderr, format, vl);
	va_end(vl);
	exit(-1);
}

void set_default_parameters(){
	developer_debug = 0;
	list_available_devices = 0;
	stat = {
		"",
		3945, 1714, 917, 1282, 478, 0,
	};
	stat_not_pushed = 1;
	raidbuff = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};
	seed = 0;
	srand((unsigned int)time(NULL));
	rng_engine = 127;
	apl = "SPELL(recklessness); SPELL(bloodthirst); SPELL(execute); SPELL(ragingblow); SPELL(wildstrike);";
	iterations = 50000;
	vary_combat_length = 20.0f;
	max_length = 450.0f;
	initial_health_percentage = 100.0f;
	death_pct = 0.0f;
	power_max = 100.0f;
	plate_specialization = 1;
	single_minded = 1;
	race = 0;
	talent = 1121311;
	mh_speed = 2.6f;
	oh_speed = 2.6f;
	mh_high = 1514;
	oh_high = 1514;
	mh_low = 814;
	oh_low = 814;
	mh_type = 1;
	oh_type = 1;

	archmages_incandescence = 0;
	archmages_greater_incandescence = 1;
	t17_2pc = 1;
	t17_4pc = 1;
	thunderlord_mh = 1;
	thunderlord_oh = 1;
	bleeding_hollow_mh = 0;
	bleeding_hollow_oh = 0;
	shattered_hand_mh = 0;
	shattered_hand_oh = 0;
}

typedef struct{
	std::string key;
	std::string value;
} kvpair_t;

void build_arglist(std::vector<kvpair_t>& arglist, int argc, char** argv){
	for (int i = 1; i < argc; i++){
		kvpair_t kv;
		char* p;
		for (p = argv[i]; *p; p++){
			if (*p == '='){
				p++;
				break;
			}
			else{
				kv.key.push_back(*p);
			}
		}
		if (*p){
			for (; *p; p++){
				kv.value.push_back(*p);
			}
			arglist.push_back(kv);
		}
		else{
			FILE* f = fopen(kv.key.c_str(), "rb");
			if (!f) { err("Parameter \"%s\" is neither a valid key-value pair, nor a exist configuration file.", kv.key.c_str()); }
			char ** rargv = (char**)calloc(1024, sizeof(char*));
			int rargc = 1;
			char buffer[1024];
			size_t buffer_i = 0;
			char ch;
			int comment = 0;
			do {
				ch = fgetc(f);
				if (!comment){
					if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF){
						if (buffer_i){
							buffer[buffer_i++] = 0;
							rargv[rargc] = (char*)malloc(buffer_i);
							memcpy(rargv[rargc], buffer, buffer_i);
							rargc++;
							buffer_i = 0;
							if (rargc >= 1024) { err("Configuration file too long."); }
						}
					}
					else if (ch == '#' && buffer_i == 0){
						comment = 1;
					}
					else{
						buffer[buffer_i++] = ch;
						if (buffer_i >= 1024) { err("Configuration line too long."); }
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
	}
}

void parse_parameters(std::vector<kvpair_t>& arglist){
	for (auto i = arglist.begin(); i != arglist.end(); i++){
		if (0 == i->key.compare("gear_str")){
			stat.gear_str = atoi(i->value.c_str());
			if (stat.gear_str < 0) stat.gear_str = 0;
			stat_not_pushed = 1;
			if (raidbuff.flask) stat.gear_str += 250;
		}
		else if (0 == i->key.compare("gear_crit")){
			stat.gear_crit = atoi(i->value.c_str());
			if (stat.gear_crit < 0) stat.gear_crit = 0;
			stat_not_pushed = 1;
			if (raidbuff.food) stat.gear_crit += 125;
		}
		else if (0 == i->key.compare("gear_mastery")){
			stat.gear_mastery = atoi(i->value.c_str());
			if (stat.gear_mastery < 0) stat.gear_mastery = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_haste")){
			stat.gear_haste = atoi(i->value.c_str());
			if (stat.gear_haste < 0) stat.gear_haste = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mult")){
			stat.gear_mult = atoi(i->value.c_str());
			if (stat.gear_mult < 0) stat.gear_mult = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_vers")){
			stat.gear_vers = atoi(i->value.c_str());
			if (stat.gear_vers < 0) stat.gear_vers = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_str+")){
			stat.gear_str += atoi(i->value.c_str());
			if (stat.gear_str < 0) stat.gear_str = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_crit+")){
			stat.gear_crit += atoi(i->value.c_str());
			if (stat.gear_crit < 0) stat.gear_crit = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mastery+")){
			stat.gear_mastery += atoi(i->value.c_str());
			if (stat.gear_mastery < 0) stat.gear_mastery = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_haste+")){
			stat.gear_haste += atoi(i->value.c_str());
			if (stat.gear_haste < 0) stat.gear_haste = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mult+")){
			stat.gear_mult += atoi(i->value.c_str());
			if (stat.gear_mult < 0) stat.gear_mult = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_vers+")){
			stat.gear_vers += atoi(i->value.c_str());
			if (stat.gear_vers < 0) stat.gear_vers = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("push_stats")){
			stat.name = i->value;
			if (stat.name.empty()) stat.name = "<unnamed stat set>";
			stat_array.push_back(stat);
			stat_not_pushed = 0;
		}
		else if (0 == i->key.compare("deterministic_seed")){
			seed = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("iterations")){
			iterations = atoi(i->value.c_str());
			if (iterations <= 0) iterations = 1;
		}
		else if (0 == i->key.compare("raidbuff_str")){
			raidbuff.str = atoi(i->value.c_str());
			raidbuff.str = !!raidbuff.str;
		}
		else if (0 == i->key.compare("raidbuff_ap")){
			raidbuff.ap = atoi(i->value.c_str());
			raidbuff.ap = !!raidbuff.ap;
		}
		else if (0 == i->key.compare("raidbuff_sp")){
			raidbuff.sp = atoi(i->value.c_str());
			raidbuff.sp = !!raidbuff.sp;
		}
		else if (0 == i->key.compare("raidbuff_crit")){
			raidbuff.crit = atoi(i->value.c_str());
			raidbuff.crit = !!raidbuff.crit;
		}
		else if (0 == i->key.compare("raidbuff_haste")){
			raidbuff.haste = atoi(i->value.c_str());
			raidbuff.haste = !!raidbuff.haste;
		}
		else if (0 == i->key.compare("raidbuff_mastery")){
			raidbuff.mastery = atoi(i->value.c_str());
			raidbuff.mastery = !!raidbuff.mastery;
		}
		else if (0 == i->key.compare("raidbuff_mult")){
			raidbuff.mult = atoi(i->value.c_str());
			raidbuff.mult = !!raidbuff.mult;
		}
		else if (0 == i->key.compare("raidbuff_vers")){
			raidbuff.vers = atoi(i->value.c_str());
			raidbuff.vers = !!raidbuff.vers;
		}
		else if (0 == i->key.compare("raidbuff_sta")){
			raidbuff.sta = atoi(i->value.c_str());
			raidbuff.sta = !!raidbuff.sta;
		}
		else if (0 == i->key.compare("raidbuff_flask")){
			int t = raidbuff.flask;
			raidbuff.flask = !!atoi(i->value.c_str());
			if (!t && raidbuff.flask) stat.gear_str += 250;
			else if (t && !raidbuff.flask) stat.gear_str -= 250;
		}
		else if (0 == i->key.compare("raidbuff_food")){
			int t = raidbuff.food;
			raidbuff.food = !!atoi(i->value.c_str());
			if (!t && raidbuff.food) stat.gear_crit += 125;
			else if (t && !raidbuff.food) stat.gear_crit -= 125;
		}
		else if (0 == i->key.compare("raidbuff_potion")){
			raidbuff.potion = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_bloodlust")){
			raidbuff.bloodlust = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_all")){
			int t1 = raidbuff.flask;
			int t2 = raidbuff.food;
			raidbuff.str = atoi(i->value.c_str());
			raidbuff.str = !!raidbuff.str;
			raidbuff.bloodlust = raidbuff.flask = raidbuff.food = raidbuff.potion = raidbuff.ap = raidbuff.crit = raidbuff.haste = raidbuff.mastery = raidbuff.mult = raidbuff.vers = raidbuff.sp = raidbuff.sta = raidbuff.str;
			raidbuff.vers = 1; /* This cannot be cancelled ingame. */
			if (!t1 && raidbuff.flask) stat.gear_str += 250;
			else if (t1 && !raidbuff.flask) stat.gear_str -= 250;
			if (!t2 && raidbuff.food) stat.gear_crit += 125;
			else if (t2 && !raidbuff.food) stat.gear_crit -= 125;
		}
		else if (0 == i->key.compare("actions")){
			apl = i->value;
			apl.append("\n");
		}
		else if (0 == i->key.compare("actions+")){
			apl.append(i->value);
			apl.append("\n");
		}
		else if (0 == i->key.compare("vary_combat_length")){
			vary_combat_length = atof(i->value.c_str());
			if (vary_combat_length > 100.0f) vary_combat_length = 100.0f;
			if (vary_combat_length < 0.0f) vary_combat_length = 0.0f;
		}
		else if (0 == i->key.compare("max_length")){
			max_length = atof(i->value.c_str());
			if (max_length < 1.0f) max_length = 1.0f;
		}
		else if (0 == i->key.compare("initial_health_percentage")){
			initial_health_percentage = atof(i->value.c_str());
			if (initial_health_percentage > 100.0f) initial_health_percentage = 100.0f;
			if (initial_health_percentage < 0.0f) initial_health_percentage = 0.0f;
		}
		else if (0 == i->key.compare("death_pct")){
			death_pct = atof(i->value.c_str());
			if (death_pct > 100.0f) death_pct = 100.0f;
			if (death_pct < 0.0f) death_pct = 0.0f;
		}
		else if (0 == i->key.compare("rage_max")){
			power_max = atof(i->value.c_str());
			if (power_max < 0.0f) power_max = 0.0f;
		}
		else if (0 == i->key.compare("plate_specialization")){
			plate_specialization = atoi(i->value.c_str());
			plate_specialization = !!plate_specialization;
		}
		else if (0 == i->key.compare("race")){
			race = -1;
			for (int j = 0; race_str_param[j]; j++){
				if (0 == i->value.compare(race_str_param[j])){
					race = j;
					break;
				}
			}
			if (race == -1) { err("No such race \"%s\".", i->value.c_str()); }
		}
		else if (0 == i->key.compare("mh_speed")){
			mh_speed = atof(i->value.c_str());
			if (mh_speed <= 0.0f) mh_speed = 1.5f;
		}
		else if (0 == i->key.compare("oh_speed")){
			oh_speed = atof(i->value.c_str());
			if (oh_speed <= 0.0f) oh_speed = 1.5f;
		}
		else if (0 == i->key.compare("mh_low")){
			mh_low = atoi(i->value.c_str());
			if (mh_low < 0) mh_low = 0;
			if (mh_high < mh_low){
				int t = mh_high;
				mh_high = mh_low;
				mh_low = t;
			}
		}
		else if (0 == i->key.compare("mh_high")){
			mh_high = atoi(i->value.c_str());
			if (mh_high < 0) mh_high = 0;
			if (mh_high < mh_low){
				int t = mh_high;
				mh_high = mh_low;
				mh_low = t;
			}
		}
		else if (0 == i->key.compare("oh_low")){
			oh_low = atoi(i->value.c_str());
			if (oh_low < 0) oh_low = 0;
			if (oh_high < oh_low){
				int t = oh_high;
				oh_high = oh_low;
				oh_low = t;
			}
		}
		else if (0 == i->key.compare("oh_high")){
			oh_high = atoi(i->value.c_str());
			if (oh_high < 0) oh_high = 0;
			if (oh_high < oh_low){
				int t = oh_high;
				oh_high = oh_low;
				oh_low = t;
			}
		}
		else if (0 == i->key.compare("mh_type")){
			if (0 == i->value.compare("2h")) mh_type = 0;
			else if (0 == i->value.compare("1h")) mh_type = 1;
			else if (0 == i->value.compare("dagger")) mh_type = 2;
			else err("No such weapon type \"%s\".", i->value.c_str());
			single_minded = (mh_type == 1 && oh_type == 1);
		}
		else if (0 == i->key.compare("oh_type")){
			if (0 == i->value.compare("2h")) oh_type = 0;
			else if (0 == i->value.compare("1h")) oh_type = 1;
			else if (0 == i->value.compare("dagger")) oh_type = 2;
			else err("No such weapon type \"%s\".", i->value.c_str());
			single_minded = (mh_type == 1 && oh_type == 1);
		}
		else if (0 == i->key.compare("talent")){
			talent = atoi(i->value.c_str());
			if (talent < 0 || talent > 3333333
				|| TALENT_TIER(1) > 3 || TALENT_TIER(2) > 3
				|| TALENT_TIER(3) > 3 || TALENT_TIER(4) > 3 || TALENT_TIER(5) > 3
				|| TALENT_TIER(6) > 3 || TALENT_TIER(7) > 3)
				err("Talent set not vaild.");
		}
		else if (0 == i->key.compare("archmages_incandescence")){
			archmages_incandescence = !!atoi(i->value.c_str());
			if (archmages_incandescence) archmages_greater_incandescence = 0;
		}
		else if (0 == i->key.compare("archmages_greater_incandescence")){
			archmages_greater_incandescence = !!atoi(i->value.c_str());
			if (archmages_greater_incandescence) archmages_incandescence = 0;
		}
		else if (0 == i->key.compare("t17_2pc")){
			t17_2pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("t17_4pc")){
			t17_4pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("mh_enchant")){
			thunderlord_mh = !i->value.compare("thunderlord");
			bleeding_hollow_mh = !i->value.compare("bleedinghollow");
			shattered_hand_mh = !i->value.compare("shatteredhand");
			if (i->value.compare("none") && !thunderlord_mh && !bleeding_hollow_mh && !shattered_hand_mh)
				err("No such weapon enchant\"%s\".", i->value.c_str());
		}
		else if (0 == i->key.compare("oh_enchant")){
			thunderlord_oh = !i->value.compare("thunderlord");
			bleeding_hollow_oh = !i->value.compare("bleedinghollow");
			shattered_hand_oh = !i->value.compare("shatteredhand");
			if (i->value.compare("none") && !thunderlord_oh && !bleeding_hollow_oh && !shattered_hand_oh)
				err("No such weapon enchant\"%s\".", i->value.c_str());
		}
		else if (0 == i->key.compare("rng_engine")){
			if (0 == i->value.compare("mt127")) rng_engine = 127;
			else if (0 == i->value.compare("mwc64x")) rng_engine = 64;
			else if (0 == i->value.compare("lcg32")) rng_engine = 32;
			else err("No such rng engine \"%s\".", i->value.c_str());
		}
		else if (0 == i->key.compare("developer_debug")){
			developer_debug = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("list_available_devices")){
			list_available_devices = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("opencl_device_id")){
			ocl().opencl_device_id = atoi(i->value.c_str());
		}
		else{
			err("Cannot parse parameter \"%s\".", i->key.c_str());
		}
	}
}

void generate_predef(){
	char buffer[256];
	predef = "";
	predef.append("#define vary_combat_length ");
	sprintf(buffer, "%ff", vary_combat_length);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define max_length ");
	sprintf(buffer, "%ff", max_length);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define initial_health_percentage ");
	sprintf(buffer, "%ff", initial_health_percentage);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define death_pct ");
	sprintf(buffer, "%ff", death_pct);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define power_max ");
	sprintf(buffer, "%ff", power_max);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define PLATE_SPECIALIZATION ");
	sprintf(buffer, "%d", plate_specialization);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define SINGLE_MINDED ");
	sprintf(buffer, "%d", single_minded);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_STR_AGI_INT ");
	sprintf(buffer, "%d", raidbuff.str);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_AP ");
	sprintf(buffer, "%d", raidbuff.ap);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_CRIT ");
	sprintf(buffer, "%d", raidbuff.crit);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_HASTE ");
	sprintf(buffer, "%d", raidbuff.haste);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_MASTERY ");
	sprintf(buffer, "%d", raidbuff.mastery);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_MULT ");
	sprintf(buffer, "%d", raidbuff.mult);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_VERS ");
	sprintf(buffer, "%d", raidbuff.vers);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_SP ");
	sprintf(buffer, "%d", raidbuff.sp);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_STA ");
	sprintf(buffer, "%d", raidbuff.sta);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_POTION ");
	sprintf(buffer, "%d", raidbuff.potion);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define BUFF_BLOODLUST ");
	sprintf(buffer, "%d", raidbuff.bloodlust);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define RACE ");
	predef.append(race_str_kernel[race]); predef.append("\r\n");

	predef.append("#define MH_LOW ");
	sprintf(buffer, "%d", mh_low);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define MH_HIGH ");
	sprintf(buffer, "%d", mh_high);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define OH_LOW ");
	sprintf(buffer, "%d", oh_low);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define OH_HIGH ");
	sprintf(buffer, "%d", oh_high);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define MH_SPEED ");
	sprintf(buffer, "%ff", mh_speed);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define OH_SPEED ");
	sprintf(buffer, "%ff", oh_speed);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define MH_TYPE ");
	predef.append(weapon_type_str[mh_type]); predef.append("\r\n");

	predef.append("#define OH_TYPE ");
	predef.append(weapon_type_str[oh_type]); predef.append("\r\n");

	predef.append("#define TALENT_TIER3 ");
	sprintf(buffer, "%d", TALENT_TIER(3));
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define TALENT_TIER4 ");
	sprintf(buffer, "%d", TALENT_TIER(4));
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define TALENT_TIER6 ");
	sprintf(buffer, "%d", TALENT_TIER(6));
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define TALENT_TIER7 ");
	sprintf(buffer, "%d", TALENT_TIER(7));
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define archmages_incandescence ");
	sprintf(buffer, "%d", archmages_incandescence);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define archmages_greater_incandescence ");
	sprintf(buffer, "%d", archmages_greater_incandescence);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define t17_2pc ");
	sprintf(buffer, "%d", t17_2pc);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define t17_4pc ");
	sprintf(buffer, "%d", t17_4pc);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define thunderlord_mh ");
	sprintf(buffer, "%d", thunderlord_mh);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define thunderlord_oh ");
	sprintf(buffer, "%d", thunderlord_oh);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define bleedinghollow_mh ");
	sprintf(buffer, "%d", bleeding_hollow_mh);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define bleedinghollow_oh ");
	sprintf(buffer, "%d", bleeding_hollow_oh);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define shatteredhand_mh ");
	sprintf(buffer, "%d", shattered_hand_mh);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define shatteredhand_oh ");
	sprintf(buffer, "%d", shattered_hand_oh);
	predef.append(buffer); predef.append("\r\n");

	if(rng_engine == 127) predef.append("#define RNG_MT127\r\n");
	else if(rng_engine == 64) predef.append("#define RNG_MWC64X\r\n");

}

int main(int argc, char** argv){
	set_default_parameters();
	std::vector<kvpair_t> arglist;
	build_arglist(arglist, argc, argv);
	parse_parameters(arglist);
	generate_predef();
	
	if (stat_not_pushed) {
		if (stat.name.empty()) stat.name = "<unnamed stat set>";
		stat_array.push_back(stat);
	}

	if (developer_debug){
		host_kernel_entry();
	}
	else if(list_available_devices){
		ocl().init();
	}
	else{
		ocl().run(apl, predef);
	}
	return 0;
}