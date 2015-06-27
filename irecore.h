#include <iostream>
#include <fstream>
#include <functional>
#include <thread>
#include <string>
#include <vector>
#include <random>
#include <cstdio>
#include <stdint.h>
#include <algorithm>
#include <chrono>
#include "VersionNo.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

typedef struct {
	std::string name;
	cl_uint gear_str;
	cl_uint gear_crit;
	cl_uint gear_haste;
	cl_uint gear_mastery;
	cl_uint gear_mult;
	cl_uint gear_vers;
	float dps;
	float dpsr;
	float dpse;
} stat_t;
typedef struct{
	int str;
	int ap;
	int sp;
	int sta;
	int crit;
	int haste;
	int mastery;
	int vers;
	int mult;
	int flask;
	int food;
	int potion;
	int bloodlust;
} raidbuff_t;
extern stat_t current_stat;
extern raidbuff_t raidbuff;
extern std::vector<stat_t> stat_array;
extern cl_uint seed;
extern int iterations;
extern int rng_engine;
extern float vary_combat_length;
extern float max_length;
extern float initial_health_percentage;
extern float death_pct;
extern float power_max; // rage_max
extern int plate_specialization;
extern int single_minded;
extern int race;
extern float mh_speed;
extern float oh_speed;
extern int mh_low, mh_high;
extern int oh_low, oh_high;
extern int mh_type;
extern int oh_type;
extern int talent;
extern int default_actions;
extern int list_available_devices;
extern int calculate_scale_factors;
extern int archmages_incandescence;
extern int archmages_greater_incandescence;
extern int legendary_ring;
extern int t17_2pc;
extern int t17_4pc;
extern int t18_2pc;
extern int t18_4pc;
extern int thunderlord_mh;
extern int thunderlord_oh;
extern int bleeding_hollow_mh;
extern int bleeding_hollow_oh;
extern int shattered_hand_mh;
extern int shattered_hand_oh;
extern int developer_debug;
extern int list_available_devices;
extern std::string trinket1_name;
extern std::string trinket2_name;
extern int trinket1_value;
extern int trinket2_value;
extern std::ostream* report_path;

class ocl_t{
private:
    cl_mem cl_res;
    cl_context context;
    cl_command_queue queue;
	cl_device_id device_used;
	int initialized;
public:
	int opencl_device_id;
    int init();
    float run(std::string& apl_cstr, std::string& predef);
    int free();
    ocl_t() : initialized(0){
    } 
    ~ocl_t(){
        //free();
    }
};
typedef struct{
	size_t platform_id;
	size_t device_id;
} pdpair_t;
typedef struct{
	std::string key;
	std::string value;
} kvpair_t;
extern std::vector<pdpair_t> ocl_device_list;
void build_arglist(std::vector<kvpair_t>& arglist, int argc, char** argv);
void parse_parameters(std::vector<kvpair_t>& arglist);
extern const char* trinket_list[];

ocl_t& ocl();

extern std::string apl;
extern std::string predef;
void parameters_consistency();
void generate_predef();
void auto_apl();

extern "C" void host_kernel_entry();

typedef struct{
	int id;
	int stat;
	int value;
} enchant_t;
typedef struct{
	int id;
	int subclass;
} item_subclass_t;
extern "C" enchant_t enchant_list[];
extern "C" enchant_t gem_list[];
extern "C" int plate_list[];
extern "C" item_subclass_t weapon_type_list[];
typedef struct{
	int type;
	int str;
	int crit;
	int haste;
	int mastery;
	int mult;
	int vers;
} item_t;
extern item_t gear_list[16];