#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <cstdio>
#include <stdint.h>
#include <algorithm>
#include <chrono>
#include <cstdarg>
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
extern std::vector<stat_t> stat_array;
extern cl_uint seed;
extern int iterations;
extern float max_length;
extern int list_available_devices;
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
        free();
    }
};

ocl_t& ocl();

extern "C" void host_kernel_entry();