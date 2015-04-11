#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cstdio>
#include <stdint.h>
#include <algorithm>
#include <chrono>
#include <cstdarg>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

typedef struct {
	cl_uint gear_str;
	cl_uint gear_crit;
	cl_uint gear_haste;
	cl_uint gear_mastery;
	cl_uint gear_mult;
	cl_uint gear_vers;
} stat_t;
extern stat_t stat;
extern cl_uint seed;
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
} raidbuff_t;

class ocl_t{
private:
    cl_mem cl_res;
    cl_context context;
    cl_command_queue queue;
    const int iterations = 100000;
	cl_device_id device_used;
public:
    int init();
    float run(std::string& apl_cstr);
    int free();
    ocl_t(){
        init();
    }
    ~ocl_t(){
        free();
    }
};

ocl_t& ocl();

extern "C" void host_kernel_entry();