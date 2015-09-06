/*
    Copyright (C) 2015 Aean(a.k.a. fhsvengetta)
    All rights reserved.

    IreCore is distributed under the terms of The MIT License.
    You should have received a copy of the MIT License along with this program.
    If not, see <http://opensource.org/licenses/mit-license.php>.
*/

#include "irecore.h"
#include "gic.h"

extern "C"{
#define LUA_BUILD_AS_DLL
#include "lua\lua.h"
#include "lua\lauxlib.h"
#include "lua\lualib.h"
};

jmp_buf panic_backpoint;
int can_reuse = 0;

// When something bad happens..
extern "C" int Panic(lua_State *L) {
    *report_path << "Panic error:" << std::endl;
    const char * msg = lua_tostring(L, 1);
	//QMessageBox::information( global_pgic, QApplication::translate( "gicClass", "Script Panic" ), QString(msg), QMessageBox::Ok );
    *report_path << msg << std::endl;
	longjmp(panic_backpoint, 1); // long jump to avoid abort crashes.
    return 1;
}

template <typename T> struct param_t{
	const char* name;
	T* pdata;
};

const param_t<int> int_param_list[] = {
	{ "gear_str" , (int*)&current_stat.gear_str },
	{ "gear_crit" , (int*)&current_stat.gear_crit },
	{ "gear_haste" , (int*)&current_stat.gear_haste },
	{ "gear_mastery" , (int*)&current_stat.gear_mastery },
	{ "gear_mult" , (int*)&current_stat.gear_mult },
	{ "gear_vers" , (int*)&current_stat.gear_vers },
	{ "deterministic_seed", (int*)&seed },
	{ "iterations", &iterations },
	{ "num_enemies", &num_enemies },
	{ "mh_low", &mh_low },
	{ "mh_high", &mh_high },
	{ "oh_low", &oh_low },
	{ "oh_high", &oh_high },
	{ "talent", &talent },
	{ "legendary_ring", &legendary_ring },
	{ "trinket1_ilvl", &trinket1_ilvl },
	{ "trinket2_ilvl", &trinket2_ilvl },
	{0}
};

const param_t<float> fp_param_list[] = {
	{ "vary_combat_length", &vary_combat_length },
	{ "max_length", &max_length },
	{ "initial_health_percentage", &initial_health_percentage },
	{ "death_pct", &death_pct },
	{ "power_max", &power_max },
	{ "mh_speed", &mh_speed },
	{ "oh_speed", &oh_speed },
	{0}
};

const param_t<int> bool_param_list[] = {
    {"raidbuff_str", &raidbuff.str },
    {"raidbuff_ap", &raidbuff.ap },
    {"raidbuff_sp", &raidbuff.sp },
    {"raidbuff_sta", &raidbuff.sta },
    {"raidbuff_crit", &raidbuff.crit },
    {"raidbuff_haste", &raidbuff.haste },
    {"raidbuff_mastery", &raidbuff.mastery },
    {"raidbuff_vers", &raidbuff.vers },
    {"raidbuff_mult", &raidbuff.mult },
    {"raidbuff_flask", &raidbuff.flask },
    {"raidbuff_food", &raidbuff.food },
    {"raidbuff_potion", &raidbuff.potion },
    {"raidbuff_bloodlust", &raidbuff.bloodlust },
	{"strict_gcd", &strict_gcd },
	{"sync_melee", &sync_melee },
	{"wbr_never_expire", &wbr_never_expire },
	{"avatar_like_bloodbath", &avatar_like_bloodbath },
	{"glyph_of_ragingwind", &glyph_of_ragingwind },
	{"plate_specialization", &plate_specialization },
	{"default_actions", &default_actions },
	{"archmages_incandescence", &archmages_incandescence },
	{"archmages_greater_incandescence", &archmages_greater_incandescence },
	{"t17_2pc", &t17_2pc },
	{"t17_4pc", &t17_4pc },
	{"t18_2pc", &t18_2pc },
	{"t18_4pc", &t18_4pc },
	{0}
};

const char* str_param_list[] = {
	"actions",
	"rng_engine",
	"trinket1",
	"trinket2",
	"mh_enchant",
	"oh_enchant",
	"mh_type",
	"oh_type",
	"race",
	0,
};

const char* reuse_param_list[] = {
	"deterministic_seed",
	"gear_str",
	"gear_crit",
	"gear_haste",
	"gear_mastery",
	"gear_mult",
	"gear_vers",
	0,
};

// Set a sim parameter.
extern "C" int SetParam(lua_State *L) {
	int n = lua_gettop(L);
	if(n != 2){
		std::string emsg = "wrong number arguments, expected 2, given ";
		emsg += n;
		lua_pushlstring(L, emsg.c_str(), emsg.length());
		lua_error(L);
	}
	if(!lua_isstring(L, 1)){
		lua_pushliteral(L, "expected string");
		lua_error(L);
	}
	std::string key = lua_tostring(L, 1);
	if(can_reuse){
		can_reuse = 0;
		for(int i = 0; reuse_param_list[i]; i++){
			if(0 == key.compare(reuse_param_list[i])){
				can_reuse = 1;
				break;
			}
		}
	}
	for(int i = 0; str_param_list[i]; i++){
		if(0 == key.compare(str_param_list[i])){
			kvpair_t pair;
			pair.key = key;
			if(!lua_isstring(L, 2)){
				lua_pushliteral(L, "expected string");
				lua_error(L);
			}
			pair.value = lua_tostring(L, 2);
			std::vector<kvpair_t> vect;
			vect.push_back(pair);
			parse_parameters(vect);
			return 0;
		}
	}
	for(int i = 0; int_param_list[i].name; i++){
		if(0 == key.compare(int_param_list[i].name)){
			if(!lua_isnumber(L, 2)){
				lua_pushliteral(L, "expected number");
				lua_error(L);
			}
			*int_param_list[i].pdata = (int)lua_tonumber(L, 2);
			return 0;
		}
	}
	for(int i = 0; fp_param_list[i].name; i++){
		if(0 == key.compare(fp_param_list[i].name)){
			if(!lua_isnumber(L, 2)){
				lua_pushliteral(L, "expected number");
				lua_error(L);
			}
			*int_param_list[i].pdata = (float)lua_tonumber(L, 2);
			return 0;
		}
	}
	for(int i = 0; bool_param_list[i].name; i++){
		if(0 == key.compare(bool_param_list[i].name)){
			*int_param_list[i].pdata = (int)lua_toboolean(L, 2);
			return 0;
		}
	}
	lua_pushliteral(L, "no such param");
	lua_error(L);
	return 0;
}

// Get a sim parameter current value.
extern "C" int GetParam(lua_State *L) {
	int n = lua_gettop(L);
	if(n != 1){
		std::string emsg = "wrong number arguments, expected 1, given ";
		emsg += n;
		lua_pushlstring(L, emsg.c_str(), emsg.length());
		lua_error(L);
	}
	if(!lua_isstring(L, 1)){
		lua_pushliteral(L, "expected string");
		lua_error(L);
	}
	std::string key = lua_tostring(L, 1);
	if(0==key.compare("actions")){
		lua_pushstring(L, apl.c_str());
		return 1;
	}
	if(0==key.compare("rng_engine")){
		switch(rng_engine){
		case 127: lua_pushstring(L, "mt127"); break;
		case 64: lua_pushstring(L, "mwc64x"); break;
		case 32: default: lua_pushstring(L, "lcg32"); break;
		}
		return 1;
	}
	if(0==key.compare("trinket1")){
		lua_pushstring(L, trinket_list[trinket1]);
		return 1;
	}
	if(0==key.compare("trinket2")){
		lua_pushstring(L, trinket_list[trinket2]);
		return 1;
	}
	if(0==key.compare("race")){
		lua_pushstring(L, race_str_param[race]);
	}
	if(0==key.compare("mh_enchant")){
		if(thunderlord_mh){
			lua_pushstring(L, "thunderlord");
			return 1;
		}
		if(bleeding_hollow_mh){
			lua_pushstring(L, "bleedinghollow");
			return 1;
		}
		if(shattered_hand_mh){
			lua_pushstring(L, "shatteredhand");
			return 1;
		}
		lua_pushstring(L, "none");
		return 1;
	}
	if(0==key.compare("oh_enchant")){
		if(thunderlord_oh){
			lua_pushstring(L, "thunderlord");
			return 1;
		}
		if(bleeding_hollow_oh){
			lua_pushstring(L, "bleedinghollow");
			return 1;
		}
		if(shattered_hand_oh){
			lua_pushstring(L, "shatteredhand");
			return 1;
		}
		lua_pushstring(L, "none");
		return 1;
	}
	if(0==key.compare("mh_type")){
		switch(mh_type){
		case 0: lua_pushstring(L, "2h"); break;
		case 1: lua_pushstring(L, "1h"); break;
		case 2: default: lua_pushstring(L, "dagger"); break;
		}
		return 1;
	}
	if(0==key.compare("oh_type")){
		switch(oh_type){
		case 0: lua_pushstring(L, "2h"); break;
		case 1: lua_pushstring(L, "1h"); break;
		case 2: default: lua_pushstring(L, "dagger"); break;
		}
		return 1;
	}
	for(int i = 0; int_param_list[i].name; i++){
		if(0 == key.compare(int_param_list[i].name)){
			lua_pushnumber(L, (lua_Number)*int_param_list[i].pdata);
			return 1;
		}
	}
	for(int i = 0; fp_param_list[i].name; i++){
		if(0 == key.compare(fp_param_list[i].name)){
			lua_pushnumber(L, (lua_Number)*fp_param_list[i].pdata);
			return 1;
		}
	}
	for(int i = 0; bool_param_list[i].name; i++){
		if(0 == key.compare(bool_param_list[i].name)){
			lua_pushboolean(L, *bool_param_list[i].pdata);
			return 1;
		}
	}
	lua_pushliteral(L, "no such param");
	lua_error(L);
	return 0;
}

// Use GUI config.
extern "C" int SetGUIParams(lua_State *L) {
	can_reuse = 0;
	set_default_parameters();
	global_pgic->set_arguments();
	global_pgic->gear_summary_calculate();
	return 0;
}

// Reset to default config.
extern "C" int ResetDefaultParams(lua_State *L) {
	can_reuse = 0;
	set_default_parameters();
	return 0;
}

// Get default APL as string.
extern "C" int GetDefaultAPL(lua_State *L) {
	std::string old_apl = apl;
	auto_apl();
	std::string new_apl = apl;
	apl = old_apl;
	lua_pushlstring(L, new_apl.c_str(), new_apl.length());
	return 1;
}

// Import From Battle.Net.
extern "C" int ImportFromBattleNet(lua_State *L) {
	std::string name, realm, region;
	int n = lua_gettop(L);
	if(n < 2){
		name = "too few arguments, expected at least 2, given ";
		name += n;
		lua_pushlstring(L, name.c_str(), name.length());
		lua_error(L);
	}
	if(!lua_isstring(L, 1)){
		lua_pushliteral(L, "expected string");
		lua_error(L);
	}
	name = lua_tostring(L, 1);
	if(!lua_isstring(L, 2)){
		lua_pushliteral(L, "expected string");
		lua_error(L);
	}
	realm = lua_tostring(L, 2);
	if(n > 2){
		if(!lua_isstring(L, 3)){
			lua_pushliteral(L, "expected string");
			lua_error(L);
		}
		region = lua_tostring(L, 3);
	}else{
		region = "cn";
	}
	int result = global_pgic->import_player(realm, name, region, 1);
	global_pgic->set_arguments();
	global_pgic->gear_summary_calculate();
	can_reuse = 0;
	lua_pushboolean(L, !result);
	return 1;
}

// start simulation
extern "C" int Run(lua_State *L){
	stat_array.clear();
	stat_not_pushed = 1;
	parameters_consistency();
    generate_predef();
	silence_mode = 1;
    ocl().run( apl, predef, can_reuse );
	can_reuse = 1;
	silence_mode = 0;
	lua_pushnumber(L, stat_array[0].dps);
	lua_pushnumber(L, stat_array[0].dpsr);
	lua_pushnumber(L, stat_array[0].dpse);
	return 3;
}

extern "C" int lua_print(lua_State* L) {
    int nargs = lua_gettop(L);

    for (int i=1; i <= nargs; i++) {
		*report_path << luaL_tolstring(L, i, NULL);
        if (i!=nargs){
            *report_path << "\t";
        }
    }
    *report_path << std::endl;

    return 0;
}

static const struct luaL_Reg printlib [] = {
    {"print", lua_print},
    {NULL, NULL}
};

#define lua_nregister(L, f) lua_register(L, #f, f)

void gic::run_scripts(){
	lua_State *L = luaL_newstate();
	if (!L) abort();
	luaL_openlibs(L);

	lua_atpanic(L, Panic);

	lua_nregister(L, SetParam);
	lua_nregister(L, GetParam);
	lua_nregister(L, SetGUIParams);
	lua_nregister(L, ResetDefaultParams);
	lua_nregister(L, GetDefaultAPL);
	lua_nregister(L, ImportFromBattleNet);
	lua_nregister(L, Run);

	lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);

	std::string script = ui.txtScript->toPlainText().toStdString();
	
	if(setjmp(panic_backpoint)){
		*report_path << QApplication::translate("gicClass", "Error occured while script executing.").toStdString() << std::endl;
		QMetaObject::invokeMethod( ui.btnRun, "setDisabled", Q_ARG( bool, false ) );
		return;
	}
	
	luaL_loadstring(L, script.c_str());
	lua_call(L, 0, 0);
	lua_close(L);
	QMetaObject::invokeMethod( ui.btnRun, "setDisabled", Q_ARG( bool, false ) );
	return;
}