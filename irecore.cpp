#include "irecore.h"

stat_t stat;
cl_uint seed;


int main(){
	stat = {
		3945, 1714, 917, 1282, 478, 0,
	};
	seed = (cl_uint)time(NULL);
	std::string str = "SPELL(bloodthirst); SPELL(execute); SPELL(ragingblow); SPELL(wildstrike);";
	std::cout << ocl().run(str) << std::endl;
	host_kernel_entry();
	return 0;
}