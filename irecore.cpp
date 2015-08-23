#include "irecore.h"

std::ostream* report_path;
stat_t current_stat;
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
int glyph_of_ragingwind;
int num_enemies;
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
int strict_gcd;
int sync_melee;
int wbr_never_expire;
int avatar_like_bloodbath;
int default_actions;
const int base10[] = { 1000000, 100000, 10000, 1000, 100, 10, 1 };
#define TALENT_TIER(tier) ((talent / base10[tier - 1]) % 10)
int calculate_scale_factors;

int archmages_incandescence;
int archmages_greater_incandescence;
int legendary_ring;
int t17_2pc;
int t17_4pc;
int t18_2pc;
int t18_4pc;
int thunderlord_mh;
int thunderlord_oh;
int bleeding_hollow_mh;
int bleeding_hollow_oh;
int shattered_hand_mh;
int shattered_hand_oh;

int developer_debug;
int list_available_devices;
int trinket1;
int trinket2;
int trinket1_ilvl;
int trinket2_ilvl;

const char* trinket_list[] = {
	"none",
	"vial_of_convulsive_shadows",
	"forgemasters_insignia",
	"horn_of_screaming_spirits",
	"scabbard_of_kyanos",
	"badge_of_victory",
	"insignia_of_victory",
	"tectus_beating_heart",
	"formidable_fang",
	"draenic_stone",
	"skull_of_war",
	"mote_of_the_mountain",
	"worldbreakers_resolve",
	"discordant_chorus",
	"empty_drinking_horn",
	"unending_hunger",
	"spores_of_alacrity",
	"bonemaws_big_toe",
	"emberscale_talisman",
	NULL
};

int trinket_scaling(int trinket, int itemlvl){
	if (trinket == 1){
		switch (itemlvl){
		case 670: return 1537;
		case 676: return 1627;
		case 685: return 1767;
		case 691: return 1870;
		case 700: return 2033;
		case 706: return 2150;
		default:  return (int)(2033.0 * pow(ilvlScaleCoeff, itemlvl - 700));
		}
	}
	else if (trinket == 2){
		switch (itemlvl){
		case 670: return 137;
		case 676: return 145;
		case 685: return 157;
		case 691: return 167;
		case 700: return 181;
		case 706: return 192;
		default:  return (int)(181.0 * pow(ilvlScaleCoeff, itemlvl - 700));
		}
	}
	else if (trinket == 3){
		switch (itemlvl){
		case 670: return 2004;
		case 676: return 2122;
		case 685: return 2304;
		case 691: return 2439;
		case 700: return 2652;
		case 706: return 2804;
		default:  return (int)(2652.0 * pow(ilvlScaleCoeff, itemlvl - 700));
		}
	}
	else if (trinket == 4){
		switch (itemlvl){
		case 665: return 2200;
		default:  return (int)(2200.0 * pow(ilvlScaleCoeff, itemlvl - 665));
		}
	}
	else if (trinket == 5){
		switch (itemlvl){
		case 620: return 456;
		case 626: return 483;
		case 660: return 728;
		case 670: return 727;
		case 680: return 798;
		case 700: return 1057;
		case 705: return 1108;
		default:  return (int)(728.0 * pow(ilvlScaleCoeff, itemlvl - 660));
		}
	}
	else if (trinket == 6){
		switch (itemlvl){
		case 620: return 543;
		case 626: return 575;
		case 660: return 867;
		case 670: return 866;
		case 680: return 951;
		case 700: return 1259;
		case 705: return 1319;
		default:  return (int)(867.0 * pow(ilvlScaleCoeff, itemlvl - 660));
		}
	}
	else if (trinket == 7){
		switch (itemlvl){
		case 655: return 1743;
		case 661: return 1843;
		case 670: return 2004;
		case 676: return 2122;
		case 685: return 2304;
		case 691: return 2439;
		default:  return (int)(2304.0 * pow(ilvlScaleCoeff, itemlvl - 685));
		}
	}
	else if (trinket == 8){
		switch (itemlvl){
		case 655: return 1743;
		default:  return (int)(1743.0 * pow(ilvlScaleCoeff, itemlvl - 655));
		}
	}
	else if (trinket == 9){
		switch (itemlvl){
		case 620: return 771;
		case 640: return 931;
		case 655: return 1069;
		case 670: return 1229;
		case 685: return 1414;
		case 700: return 1627;
		case 715: return 1870;
		default:  return (int)(1414.0 * pow(ilvlScaleCoeff, itemlvl - 685));
		}
	}
	else if (trinket == 10){
		switch (itemlvl){
		case 640: return 1396;
		case 655: return 1604;
		case 670: return 1844;
		case 685: return 2120;
		case 700: return 2440;
		case 715: return 2804;
		default:  return (int)(2804.0 * pow(ilvlScaleCoeff, itemlvl - 715));
		}
	}
	else if (trinket == 11){
		switch (itemlvl){
		case 640: return 1517;
		case 646: return 1604;
		default:  return (int)(1517.0 * pow(ilvlScaleCoeff, itemlvl - 640));
		}
	}
	else if (trinket == 12){
		switch (itemlvl){
		case 705: return 220;
		case 711: return 232;
		case 720: return 253;
		case 726: return 267;
		case 735: return 290;
		case 741: return 307;
		default:  return (int)(290.0 * pow(ilvlScaleCoeff, itemlvl - 735));
		}
	}
	else if (trinket == 13){
		switch (itemlvl){
		case 700: return 20564;
		case 706: return 21744;
		case 715: return 23632;
		case 721: return 24981;
		case 730: return 27172;
		case 736: return 28757;
		default:  return (int)(27172.0 * pow(ilvlScaleCoeff, itemlvl - 730));
		}
	}
	else if (trinket == 14){
		switch (itemlvl){
		case 700: return 259;
		case 706: return 274;
		case 715: return 298;
		case 721: return 315;
		case 730: return 342;
		case 736: return 362;
		default:  return (int)(342.0 * pow(ilvlScaleCoeff, itemlvl - 730));
		}
	}
	else if (trinket == 15){
		switch (itemlvl){
		case 695: return 54;
		case 701: return 57;
		case 710: return 62;
		case 716: return 65;
		case 725: return 71;
		case 731: return 75;
		default:  return (int)(71.0 * pow(ilvlScaleCoeff, itemlvl - 725));
		}
	}
	else if (trinket == 16){
		switch (itemlvl){
		case 685: return 2304;
		case 705: return 2778;
		case 630: return 1383;
		case 636: return 1461;
		case 615: return 1200;
		case 600: return 1043;
		case 570: return 791;
		case 550: return 657;
		case 530: return 543;
		default:  return (int)(1383.0 * pow(ilvlScaleCoeff, itemlvl - 630));
		}
	}
	else if (trinket == 17){
		switch (itemlvl){
		case 685: return 1767;
		case 705: return 2130;
		case 630: return 1060;
		case 636: return 1120;
		case 615: return 920;
		case 600: return 800;
		case 570: return 607;
		case 550: return 503;
		case 530: return 417;
		default:  return (int)(1060.0 * pow(ilvlScaleCoeff, itemlvl - 630));
		}
	}
	else if (trinket == 18){
		switch (itemlvl){
		case 685: return 1767;
		case 705: return 2130;
		case 630: return 1060;
		case 636: return 1120;
		case 615: return 920;
		case 600: return 800;
		case 570: return 607;
		case 550: return 503;
		case 530: return 417;
		default:  return (int)(1060.0 * pow(ilvlScaleCoeff, itemlvl - 630));
		}
	}
	else return 0;
}
int trinket_from_id(int id){
	switch (id){
	case 113969: return 1;
	case 113983: return 2;
	case 119193: return 3;
	case 118882: return 4;
	case 115159: case 119936: case 111232: case 115759: case 125041: case 125518: case 126632: case 126155: case 124867: case 125344: case 125981: case 126458: return 5;
	case 115160: case 119937: case 111233: case 115760: case 125042: case 125519: case 126633: case 126156: case 124868: case 125345: case 125982: case 126459: return 6;
	case 113645: return 7;
	case 114613: return 8;
	case 109262: case 122601: case 122602: case 122603: case 122604: case 128023: case 128024: return 9;
	case 112318: return 10;
	case 116292: return 11;
	case 124523: return 12;
	case 124237: return 13;
	case 124238: return 14;
	case 124236: return 15;
	case 110014: return 16;
	case 110012: return 17;
	case 110013: return 18;
	default: return 0;
	}
}

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

void set_default_parameters(){
	developer_debug = 0;
	list_available_devices = 0;
	current_stat = {
		"",
		0,
	};
	stat_not_pushed = 1;
	raidbuff = {
		0
	};
	raidbuff.vers = 1;
	seed = 0;
	srand((unsigned int)time(NULL));
	rng_engine = 32;
	strict_gcd = 1;
	sync_melee = 1;
	wbr_never_expire = 1;
	avatar_like_bloodbath = 0;
	apl = "";
	default_actions=0;
	iterations = 50000;
	vary_combat_length = 20.0f;
	max_length = 450.0f;
	initial_health_percentage = 100.0f;
	death_pct = 0.0f;
	power_max = 100.0f;
	glyph_of_ragingwind = 0;
	num_enemies = 1;
	plate_specialization = 0;
	single_minded = 0;
	race = 0;
	talent = 0;
	mh_speed = 1.5f;
	oh_speed = 1.5f;
	mh_high = 0;
	oh_high = 0;
	mh_low = 0;
	oh_low = 0;
	mh_type = 2;
	oh_type = 2;

	archmages_incandescence = 0;
	archmages_greater_incandescence = 0;
	legendary_ring = 0;
	t17_2pc = 0;
	t17_4pc = 0;
	t18_2pc = 0;
	t18_4pc = 0;
	thunderlord_mh = 0;
	thunderlord_oh = 0;
	bleeding_hollow_mh = 0;
	bleeding_hollow_oh = 0;
	shattered_hand_mh = 0;
	shattered_hand_oh = 0;

	trinket1 = 0;
	trinket2 = 0;

	report_path = &std::cout;
	calculate_scale_factors = 0;
}

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
			if (!f) { *report_path << "Parameter \"" << kv.key << "\" is neither a valid key-value pair, nor a exist configuration file." << std::endl; continue; }
			char ** rargv = (char**)calloc(65536, sizeof(char*));
			int rargc = 1;
			char buffer[4096];
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
							if (rargc >= 65536) { *report_path << "Configuration file too long." << std::endl; break; }
						}
					}
					else if (ch == '#' && buffer_i == 0){
						comment = 1;
					}
					else{
						buffer[buffer_i++] = ch;
						if (buffer_i >= 4096) { *report_path << "Configuration line too long." << std::endl; break; }
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
			current_stat.gear_str = atoi(i->value.c_str());
			if (current_stat.gear_str < 0) current_stat.gear_str = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_crit")){
			current_stat.gear_crit = atoi(i->value.c_str());
			if (current_stat.gear_crit < 0) current_stat.gear_crit = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mastery")){
			current_stat.gear_mastery = atoi(i->value.c_str());
			if (current_stat.gear_mastery < 0) current_stat.gear_mastery = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_haste")){
			current_stat.gear_haste = atoi(i->value.c_str());
			if (current_stat.gear_haste < 0) current_stat.gear_haste = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mult")){
			current_stat.gear_mult = atoi(i->value.c_str());
			if (current_stat.gear_mult < 0) current_stat.gear_mult = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_vers")){
			current_stat.gear_vers = atoi(i->value.c_str());
			if (current_stat.gear_vers < 0) current_stat.gear_vers = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_str+")){
			current_stat.gear_str += atoi(i->value.c_str());
			if (current_stat.gear_str < 0) current_stat.gear_str = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_crit+")){
			current_stat.gear_crit += atoi(i->value.c_str());
			if (current_stat.gear_crit < 0) current_stat.gear_crit = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mastery+")){
			current_stat.gear_mastery += atoi(i->value.c_str());
			if (current_stat.gear_mastery < 0) current_stat.gear_mastery = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_haste+")){
			current_stat.gear_haste += atoi(i->value.c_str());
			if (current_stat.gear_haste < 0) current_stat.gear_haste = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mult+")){
			current_stat.gear_mult += atoi(i->value.c_str());
			if (current_stat.gear_mult < 0) current_stat.gear_mult = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_vers+")){
			current_stat.gear_vers += atoi(i->value.c_str());
			if (current_stat.gear_vers < 0) current_stat.gear_vers = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_str-")){
			current_stat.gear_str -= atoi(i->value.c_str());
			if (current_stat.gear_str < 0) current_stat.gear_str = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_crit-")){
			current_stat.gear_crit -= atoi(i->value.c_str());
			if (current_stat.gear_crit < 0) current_stat.gear_crit = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mastery-")){
			current_stat.gear_mastery -= atoi(i->value.c_str());
			if (current_stat.gear_mastery < 0) current_stat.gear_mastery = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_haste-")){
			current_stat.gear_haste -= atoi(i->value.c_str());
			if (current_stat.gear_haste < 0) current_stat.gear_haste = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_mult-")){
			current_stat.gear_mult -= atoi(i->value.c_str());
			if (current_stat.gear_mult < 0) current_stat.gear_mult = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("gear_vers-")){
			current_stat.gear_vers -= atoi(i->value.c_str());
			if (current_stat.gear_vers < 0) current_stat.gear_vers = 0;
			stat_not_pushed = 1;
		}
		else if (0 == i->key.compare("push_stats")){
			current_stat.name = i->value;
			if (current_stat.name.empty()) current_stat.name = "<unnamed stat set>";
			stat_array.push_back(current_stat);
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
			raidbuff.flask = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_food")){
			raidbuff.food = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_potion")){
			raidbuff.potion = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_bloodlust")){
			raidbuff.bloodlust = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("raidbuff_all")){
			raidbuff.str = atoi(i->value.c_str());
			raidbuff.str = !!raidbuff.str;
			raidbuff.bloodlust = raidbuff.flask = raidbuff.food = raidbuff.potion = raidbuff.ap = raidbuff.crit = raidbuff.haste = raidbuff.mastery = raidbuff.mult = raidbuff.vers = raidbuff.sp = raidbuff.sta = raidbuff.str;
			raidbuff.vers = 1; /* This cannot be cancelled ingame. */
			
		}
		else if (0 == i->key.compare("actions")){
			apl = i->value;
			apl.append("\n");
		}
		else if (0 == i->key.compare("actions+")){
			apl.append(i->value);
			apl.append("\n");
		}
		else if (0 == i->key.compare("default_actions")){
			default_actions = !!atoi(i->value.c_str());
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
		else if (0 == i->key.compare("num_enemies")){
			num_enemies = atof(i->value.c_str());
			if (num_enemies < 1) num_enemies = 1;
		}
		else if (0 == i->key.compare("glyph_of_ragingwind")){
			glyph_of_ragingwind = atof(i->value.c_str());
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
			if (race == -1) { *report_path << "No such race \"" << i->value << "\"." << std::endl; race = 0; }
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
		}
		else if (0 == i->key.compare("mh_high")){
			mh_high = atoi(i->value.c_str());
			if (mh_high < 0) mh_high = 0;
		}
		else if (0 == i->key.compare("oh_low")){
			oh_low = atoi(i->value.c_str());
			if (oh_low < 0) oh_low = 0;
		}
		else if (0 == i->key.compare("oh_high")){
			oh_high = atoi(i->value.c_str());
			if (oh_high < 0) oh_high = 0;
		}
		else if (0 == i->key.compare("mh_type")){
			if (0 == i->value.compare("2h")) mh_type = 0;
			else if (0 == i->value.compare("1h")) mh_type = 1;
			else if (0 == i->value.compare("dagger")) mh_type = 2;
			else *report_path << "No such weapon type \"" << i->value << "\"." << std::endl;
		}
		else if (0 == i->key.compare("oh_type")){
			if (0 == i->value.compare("2h")) oh_type = 0;
			else if (0 == i->value.compare("1h")) oh_type = 1;
			else if (0 == i->value.compare("dagger")) oh_type = 2;
			else *report_path << "No such weapon type \"" << i->value << "\"." << std::endl;
		}
		else if (0 == i->key.compare("talent")){
			talent = atoi(i->value.c_str());
			if (talent < 0 || talent > 3333333
				|| TALENT_TIER(1) > 3 || TALENT_TIER(2) > 3
				|| TALENT_TIER(3) > 3 || TALENT_TIER(4) > 3 || TALENT_TIER(5) > 3
				|| TALENT_TIER(6) > 3 || TALENT_TIER(7) > 3){
				*report_path << "Talent set not vaild.";
				talent = 0000000;
			}
		}
		else if (0 == i->key.compare("archmages_incandescence")){
			archmages_incandescence = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("archmages_greater_incandescence")){
			archmages_greater_incandescence = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("legendary_ring")){
			int itemlvl = atoi(i->value.c_str());
			legendary_ring = 2500.0 * pow(ilvlScaleCoeff, itemlvl - 735);
		}
		else if (0 == i->key.compare("t17_2pc")){
			t17_2pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("t17_4pc")){
			t17_4pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("t18_2pc")){
			t18_2pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("t18_4pc")){
			t18_4pc = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("mh_enchant")){
			thunderlord_mh = !i->value.compare("thunderlord");
			bleeding_hollow_mh = !i->value.compare("bleedinghollow");
			shattered_hand_mh = !i->value.compare("shatteredhand");
			if (i->value.compare("none") && !thunderlord_mh && !bleeding_hollow_mh && !shattered_hand_mh)
				*report_path << "No such weapon enchant\"" << i->value << "\"." << std::endl;
		}
		else if (0 == i->key.compare("oh_enchant")){
			thunderlord_oh = !i->value.compare("thunderlord");
			bleeding_hollow_oh = !i->value.compare("bleedinghollow");
			shattered_hand_oh = !i->value.compare("shatteredhand");
			if (i->value.compare("none") && !thunderlord_oh && !bleeding_hollow_oh && !shattered_hand_oh)
				*report_path << "No such weapon enchant\"" << i->value << "\"." << std::endl;
		}
		else if (0 == i->key.compare("trinket1")){
			char* buf = new char[i->value.size()+6];
			char* p;
			strcpy(buf, i->value.c_str());
			for (p = buf; *p; p++){
				if (*p == ','){
					*p++ = 0;
					break;
				}
			}
			if (!*p || p[0]!='i' || p[1]!='l' || p[2]!='v' || p[3]!='l' || p[4]!='=')
				if (strcmp(buf, "none")){ *report_path << "Unexpected trinket grammar. Correct grammar:\n\ttrinket1=trinket_name,ilvl=123\n\ttrinket1=none" << std::endl; continue; }
			std::string trinket1_name = buf;
			if(strcmp(buf,"none"))
				trinket1_ilvl = atoi(p+5);
			int x;
			for (x = 0; trinket_list[x]; x++){
				if (0 == trinket1_name.compare(trinket_list[x])){
					trinket1 = x;
					break;
				}
			}
			if (!trinket_list[x]){ *report_path << "No such trinket \"" << trinket1_name << "\"." << std::endl; trinket1 = 0; continue; }
			if (trinket1 == trinket2 && 0 != trinket1){ *report_path << "Duplicated trinkets \"" << trinket1_name << "\" not allowed." << std::endl; trinket1 = 0; continue; }
			delete[] buf;
		}
		else if (0 == i->key.compare("trinket2")){
			char* buf = new char[i->value.size()+6];
			char* p;
			strcpy(buf, i->value.c_str());
			for (p = buf; *p; p++){
				if (*p == ','){
					*p++ = 0;
					break;
				}
			}
			if (!*p || p[0] != 'i' || p[1] != 'l' || p[2] != 'v' || p[3] != 'l' || p[4] != '=')
				if (strcmp(buf, "none")){ *report_path << "Unexpected trinket grammar. Correct grammar:\n\ttrinket2=trinket_name,ilvl=123\n\ttrinket2=none" << std::endl; continue; }
			std::string trinket2_name = buf;
			if(strcmp(buf,"none"))
				trinket2_ilvl = atoi(p+5);
			int x;
			for (x = 0; trinket_list[x]; x++){
				if (0 == trinket2_name.compare(trinket_list[x])){
					trinket2 = x;
					break;
				}
			}
			if (!trinket_list[x]){ *report_path << "No such trinket \"" << trinket2_name << "\"." << std::endl; trinket2 = 0; continue; }
			if (trinket1 == trinket2 && 0 != trinket2){ *report_path << "Duplicated trinkets \"" << trinket2_name << "\" not allowed." << std::endl; trinket2 = 0; continue; }
			delete[] buf;
		}
		else if (0 == i->key.compare("rng_engine")){
			if (0 == i->value.compare("mt127")) rng_engine = 127;
			else if (0 == i->value.compare("mwc64x")) rng_engine = 64;
			else if (0 == i->value.compare("lcg32")) rng_engine = 32;
			else *report_path << "No such rng engine \"" << i->value << "\"." << std::endl;
		}
		else if (0 == i->key.compare("strict_gcd")){
			strict_gcd = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("wbr_never_expire")){
			wbr_never_expire = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("sync_melee")){
			sync_melee = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("avatar_like_bloodbath")){
			avatar_like_bloodbath = !!atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("support_non_c99")){
			*report_path << "The option \"support_non_c99\" has been deprecated since irecore-620-49. IreCore should works well on all main-stream platforms now." << std::endl;
		}
		else if (0 == i->key.compare("output")){
			report_path = new std::ofstream(i->value.c_str(), std::ofstream::out);
		}
		else if (0 == i->key.compare("calculate_scale_factors")){
			calculate_scale_factors = !!atoi(i->value.c_str());
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
			*report_path << "Cannot parse parameter \"" << i->key << "\"." << std::endl;
		}
	}
}

void generate_predef(){
	char buffer[256];
	predef = "";

	predef.append("#define STRICT_GCD ");
	sprintf(buffer, "%d", strict_gcd);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define SYNC_MELEE ");
	sprintf(buffer, "%d", sync_melee);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define WBR_NEVER_EXPIRE ");
	sprintf(buffer, "%d", wbr_never_expire);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define AVATAR_LIKE_BLOODBATH ");
	sprintf(buffer, "%d", avatar_like_bloodbath);
	predef.append(buffer); predef.append("\r\n");

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

	predef.append("#define num_enemies ");
	sprintf(buffer, "%d", num_enemies);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define GLYPH_OF_RAGINGWIND ");
	sprintf(buffer, "%d", glyph_of_ragingwind);
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

	predef.append("#define legendary_ring ");
	sprintf(buffer, "%d", legendary_ring);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define t17_2pc ");
	sprintf(buffer, "%d", t17_2pc);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define t17_4pc ");
	sprintf(buffer, "%d", t17_4pc);
	predef.append(buffer); predef.append("\r\n");
	
	predef.append("#define t18_2pc ");
	sprintf(buffer, "%d", t18_2pc);
	predef.append(buffer); predef.append("\r\n");

	predef.append("#define t18_4pc ");
	sprintf(buffer, "%d", t18_4pc);
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

	if (trinket1){
		predef.append("#define trinket_");
		predef.append(trinket_list[trinket1]);
		predef.append(" ");
		sprintf(buffer, "%d", trinket_scaling(trinket1, trinket1_ilvl));
		predef.append(buffer);
		predef.append("\r\n");
	}
	if (trinket2){
		predef.append("#define trinket_");
		predef.append(trinket_list[trinket2]);
		predef.append(" ");
		sprintf(buffer, "%d", trinket_scaling(trinket2, trinket2_ilvl));
		predef.append(buffer);
		predef.append("\r\n");
	}
}

void auto_apl(){
	apl = "if(!UP(enrage.expire)||(REMAIN(bloodthirst.cd)>FROM_SECONDS(3)&&rti->player.ragingblow.stack<2))SPELL(berserkerrage);\n";
	apl.append("if((((num_enemies>1.000000f)||!0)&&(((T63(1,0)&&(T63(TO_SECONDS(REMAIN(bladestorm.cd)),0)==0.000000f))||UP(recklessness.expire))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<25.000000f))))LEGENDARY(SPELL(thorasus_the_stone_heart_of_draenor),0);\n");
	apl.append("if((((enemy_health_percent(rti)<20.000000f)&&UP(recklessness.expire))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<=30.000000f)))POTION(SPELL(potion),0);\n");

	if (0 == strcmp(trinket_list[trinket1], "vial_of_convulsive_shadows") || 0 == strcmp(trinket_list[trinket2], "vial_of_convulsive_shadows"))
		if (TALENT_TIER(7) == 1) apl.append("if(UP(recklessness.expire)||TIME_DISTANT(rti->expected_combat_length)<FROM_SECONDS(25))SPELL(vial_of_convulsive_shadows);\n");
		else apl.append("SPELL(vial_of_convulsive_shadows);\n");
	if (0 == strcmp(trinket_list[trinket1], "bonemaws_big_toe") || 0 == strcmp(trinket_list[trinket2], "bonemaws_big_toe"))
		if (TALENT_TIER(7) == 1) apl.append("if(UP(recklessness.expire)||TIME_DISTANT(rti->expected_combat_length)<FROM_SECONDS(25))SPELL(bonemaws_big_toe);\n");
		else apl.append("SPELL(bonemaws_big_toe);\n");
	if (0 == strcmp(trinket_list[trinket1], "scabbard_of_kyanos") || 0 == strcmp(trinket_list[trinket2], "scabbard_of_kyanos")) apl.append("SPELL(scabbard_of_kyanos);\n");
	if (0 == strcmp(trinket_list[trinket1], "emberscale_talisman") || 0 == strcmp(trinket_list[trinket2], "emberscale_talisman")) apl.append("SPELL(emberscale_talisman);\n");
	if (0 == strcmp(trinket_list[trinket1], "badge_of_victory") || 0 == strcmp(trinket_list[trinket2], "badge_of_victory")) apl.append("SPELL(badge_of_victory);\n");
	
	apl.append("if((((((T62(UP(bloodbath.expire),0)||(T62(TO_SECONDS(REMAIN(bloodbath.cd)),0)>25.000000f))||!T62(1,0))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<15.000000f))&&((T63(1,0)&&(!0||(num_enemies==1.000000f)))||!T63(1,0)))&&t18_4pc))SPELL(recklessness);\n");
	apl.append("if(((T71(1,0)&&((T63(1,0)&&(!0||(num_enemies==1.000000f)))||!T63(1,0)))&&!t18_4pc)){\n");
	apl.append("if((((TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))>140.000000f)||(enemy_health_percent(rti)<20.000000f))&&((T62(UP(bloodbath.expire),0)||!T62(1,0))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<15.000000f))))SPELL(recklessness);\n");
	apl.append("}\n");
	apl.append("if(((!T71(1,0)&&((T63(1,0)&&(!0||(num_enemies==1.000000f)))||!T63(1,0)))&&!t18_4pc)){\n");
	apl.append("if((((TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))>190.000000f)||(enemy_health_percent(rti)<20.000000f))&&((T62(UP(bloodbath.expire),0)||!T62(1,0))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<15.000000f))))SPELL(recklessness);\n");
	apl.append("}\n");
	apl.append("if(((UP(recklessness.expire)||(TO_SECONDS(REMAIN(recklessness.cd))>60.000000f))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<30.000000f)))T61(SPELL(avatar),0);\n");
	apl.append("if(((T62(UP(bloodbath.expire),0)||!T62(1,0))||UP(recklessness.expire)))ORC(SPELL(bloodfury),0);\n");
	apl.append("if(((T62(UP(bloodbath.expire),0)||!T62(1,0))||UP(recklessness.expire)))TROLL(SPELL(berserking),0);\n");
	apl.append("if((rti->player.power<(power_max-40.000000f)))BELF(SPELL(arcane_torrent),0);\n");
	apl.append("if((num_enemies==2.000000f)){\n");
	apl.append("T62(SPELL(bloodbath),0);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T72(SPELL(ravager),0);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T43(SPELL(dragonroar),0);\n");
	apl.append("{\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))SPELL(recklessness);\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))T63(SPELL(bladestorm),0);\n");
	apl.append("}\n");
	apl.append("if(((!UP(enrage.expire)||(rti->player.power<40.000000f))||!UP(ragingblow.expire)))SPELL(bloodthirst);\n");
	apl.append("T73(SPELL(siegebreaker),0);\n");
	apl.append("SPELL(execute);\n");
	apl.append("if((UP(meatcleaver.expire)||(enemy_health_percent(rti)<20.000000f)))SPELL(ragingblow);\n");
	apl.append("if((!UP(meatcleaver.expire)&&(enemy_health_percent(rti)>20.000000f)))SPELL(whirlwind);\n");
	apl.append("if(UP(bloodsurge.expire))SPELL(wildstrike);\n");
	apl.append("SPELL(bloodthirst);\n");
	apl.append("SPELL(whirlwind);\n");
	apl.append("}\n");
	apl.append("if((num_enemies==3.000000f)){\n");
	apl.append("T62(SPELL(bloodbath),0);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T72(SPELL(ravager),0);\n");
	apl.append("{\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))SPELL(recklessness);\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))T63(SPELL(bladestorm),0);\n");
	apl.append("}\n");
	apl.append("if(((!UP(enrage.expire)||(rti->player.power<50.000000f))||!UP(ragingblow.expire)))SPELL(bloodthirst);\n");
	apl.append("if((rti->player.meatcleaver.stack>=2.000000f))SPELL(ragingblow);\n");
	apl.append("T73(SPELL(siegebreaker),0);\n");
	apl.append("SPELL(execute);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T43(SPELL(dragonroar),0);\n");
	apl.append("if((enemy_health_percent(rti)>20.000000f))SPELL(whirlwind);\n");
	apl.append("SPELL(bloodthirst);\n");
	apl.append("if(UP(bloodsurge.expire))SPELL(wildstrike);\n");
	apl.append("SPELL(ragingblow);\n");
	apl.append("}\n");
	apl.append("if((num_enemies>3.000000f)){\n");
	apl.append("T62(SPELL(bloodbath),0);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T72(SPELL(ravager),0);\n");
	apl.append("if(((rti->player.meatcleaver.stack>=3.000000f)&&UP(enrage.expire)))SPELL(ragingblow);\n");
	apl.append("if(((!UP(enrage.expire)||(rti->player.power<50.000000f))||!UP(ragingblow.expire)))SPELL(bloodthirst);\n");
	apl.append("if((rti->player.meatcleaver.stack>=3.000000f))SPELL(ragingblow);\n");
	apl.append("{\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))SPELL(recklessness);\n");
	apl.append("if(((TO_SECONDS(REMAIN(enrage.expire))>6.000000f)&&((((T71(1,0)&&(0>45.000000f))||(!T71(1,0)&&(0>60.000000f)))||!0)||(num_enemies>num_enemies))))T63(SPELL(bladestorm),0);\n");
	apl.append("}\n");
	apl.append("SPELL(whirlwind);\n");
	apl.append("T73(SPELL(siegebreaker),0);\n");
	apl.append("if(T32(UP(suddendeath.expire),0))SPELL(execute);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T43(SPELL(dragonroar),0);\n");
	apl.append("SPELL(bloodthirst);\n");
	apl.append("if(UP(bloodsurge.expire))SPELL(wildstrike);\n");
	apl.append("}\n");
	apl.append("{\n");
	apl.append("T62(SPELL(bloodbath),0);\n");
	apl.append("if(((enemy_health_percent(rti)<20.000000f)&&0))SPELL(recklessness);\n");
	apl.append("if(((rti->player.power>(power_max-20.000000f))&&(enemy_health_percent(rti)>20.000000f)))SPELL(wildstrike);\n");
	apl.append("if((((!T33(1,0)&&(rti->player.power<(power_max-40.000000f)))||!UP(enrage.expire))||(rti->player.ragingblow.stack<2.000000f)))SPELL(bloodthirst);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||(!T62(1,0)&&((!0||(0>60.000000f))||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<40.000000f)))))T72(SPELL(ravager),0);\n");
	apl.append("T73(SPELL(siegebreaker),0);\n");
	apl.append("if(T32(UP(suddendeath.expire),0))SPELL(execute);\n");
	apl.append("T41(SPELL(stormbolt),0);\n");
	apl.append("if(UP(bloodsurge.expire))SPELL(wildstrike);\n");
	apl.append("if((UP(enrage.expire)||(TO_SECONDS(TIME_DISTANT(rti->expected_combat_length))<12.000000f)))SPELL(execute);\n");
	apl.append("if((T62(UP(bloodbath.expire),0)||!T62(1,0)))T43(SPELL(dragonroar),0);\n");
	apl.append("SPELL(ragingblow);\n");
	apl.append("if(((TO_SECONDS(REMAIN(bloodthirst.cd))<0.500000f)&&(rti->player.power<50.000000f)))return;\n");
	apl.append("if((UP(enrage.expire)&&(enemy_health_percent(rti)>20.000000f)))SPELL(wildstrike);\n");
	apl.append("if(!0)T63(SPELL(bladestorm),0);\n");
	apl.append("if(!T33(1,0))T42(SPELL(shockwave),0);\n");
	apl.append("SPELL(bloodthirst);\n");
	apl.append("}\n");

}

void parameters_consistency(){
	single_minded = (mh_type == 1 && oh_type == 1);
	if (trinket1 == trinket2 && 0 != trinket1){
		*report_path << "Duplicated trinkets \"" << trinket_list[trinket2] << "\" not allowed. Trinket 2 reset to none." << std::endl;
		trinket2 = 0;
	}
	if (mh_high < mh_low){
		*report_path << "MH Low Damage is higher than MH High Damage, exchanged." << std::endl;
		int t = mh_high;
		mh_high = mh_low;
		mh_low = t;
	}
	if (oh_high < oh_low){
		*report_path << "OH Low Damage is higher than OH High Damage, exchanged." << std::endl;
		int t = oh_high;
		oh_high = oh_low;
		oh_low = t;
	}
	if (mh_speed <= .0){
		*report_path << "MH Speed less than or equal to zero, reset to 1.5s." << std::endl;
		mh_speed = 1.5;
	}
	if (oh_speed <= .0){
		*report_path << "OH Speed less than or equal to zero, reset to 1.5s." << std::endl;
		oh_speed = 1.5;
	}
	if (num_enemies < 1){
		*report_path << "Num Enemies less than 1, reset to 1." << std::endl;
		num_enemies = 1;
	}
	if (num_enemies > 20){
		*report_path << "Num Enemies greater than 20, reset to 20." << std::endl;
		num_enemies = 20;
	}
	if (stat_not_pushed) {
		if (current_stat.name.empty()) current_stat.name = "<unnamed stat set>";
		stat_array.push_back(current_stat);
		stat_not_pushed = 0;
	}
	if (raidbuff.flask){
		for (auto i = stat_array.begin(); i != stat_array.end(); i++)
		i->gear_str += 250;
	}
	if (raidbuff.food){
		for (auto i = stat_array.begin(); i != stat_array.end(); i++)
			i->gear_crit += 125 * (race == 14 ? 2 : 1);
	}
	if (default_actions){
		auto_apl();
	}
	if (calculate_scale_factors){
		if (stat_array.size() > 1) {
			*report_path << "Scale factors enabled while multiple stat sets given. Stat sets except the first set are abbandoned." << std::endl;
		}
		current_stat = stat_array[0];
		stat_array.clear();
		current_stat.name = "scale factors baseline";
		stat_array.push_back(current_stat);

		current_stat.name = "scale factors str";
		current_stat.gear_str += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_str -= 120;

		current_stat.name = "scale factors crit";
		current_stat.gear_crit += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_crit -= 120;

		current_stat.name = "scale factors haste";
		current_stat.gear_haste += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_haste -= 120;

		current_stat.name = "scale factors mastery";
		current_stat.gear_mastery += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_mastery -= 120;

		current_stat.name = "scale factors mult";
		current_stat.gear_mult += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_mult -= 120;

		current_stat.name = "scale factors vers";
		current_stat.gear_vers += 120;
		stat_array.push_back(current_stat);
		current_stat.gear_vers -= 120;
	}
}

int clic_main(int argc, char** argv){
	std::cout << "IreCore " << STRFILEVER << " " << __DATE__ << "\n" << std::endl;
	set_default_parameters();
	std::vector<kvpair_t> arglist;
	build_arglist(arglist, argc, argv);
	parse_parameters(arglist);
	parameters_consistency();
	generate_predef();

	if (developer_debug){
		std::cout << predef << std::endl;
		host_kernel_entry();
	}
	else if(list_available_devices){
		ocl().init();
	}
	else{
		ocl().run(apl, predef);
	}

	if (calculate_scale_factors){
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
		}

	}

	report_path->flush();
	return 0;
}