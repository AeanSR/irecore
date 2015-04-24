#include "irecore.h"

char* ocl_src_char;

typedef struct{
	size_t platform_id;
	size_t device_id;
} pdpair_t;
std::vector<pdpair_t> ocl_device_list;

template <typename T>
T prefix_mean(const T* _data, size_t n) {
    /* Make a copy of _data. */
    std::vector<T> data(_data, _data + n);

    /* Prefix-sum to minimize rounding errors for FP types. */
    for (size_t k = 1; k < data.size(); k += k) {
        for (size_t i = 0; i < data.size(); i += k) {
            auto j = i + k;
            if (j < data.size()) {
                data[i] += data[j];
                i = j;
            }
            else {
                break;
            }
        }
    }

    return data[0] / static_cast<T>(n);
}
template <typename T>
T prefix_stddev(const T* _data, size_t n, T mean) {
    /* Make a copy of _data. */
    std::vector<T> data(_data, _data + n);

	for (size_t i = 0; i < data.size(); i++)
		data[i] = (data[i] - mean) * (data[i] - mean);

    /* Prefix-sum to minimize rounding errors for FP types. */
    for (size_t k = 1; k < data.size(); k += k) {
        for (size_t i = 0; i < data.size(); i += k) {
            auto j = i + k;
            if (j < data.size()) {
                data[i] += data[j];
                i = j;
            }
            else {
                break;
            }
        }
    }

    return sqrt( data[0] / static_cast<T>(n) );
}

int ocl_t::init()
{
	std::cout << "Query available compute devices ...\n";

	cl_int err;
	cl_uint num;
	err = clGetPlatformIDs(0, 0, &num);
	if (err != CL_SUCCESS) {
		std::cerr << "Unable to get platforms\n";
		return 0;
	}

	std::vector<cl_platform_id> platforms(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if (err != CL_SUCCESS) {
		std::cerr << "Unable to get platform ID\n";
		return 0;
	}

	int device_counter = 0;
	for (size_t platform_id = 0; platform_id < num; platform_id++){
		size_t dev_c, info_c;
		clGetPlatformInfo(platforms[platform_id], CL_PLATFORM_NAME, 0, NULL, &info_c);
		std::string platname;
		platname.resize(info_c);
		clGetPlatformInfo(platforms[platform_id], CL_PLATFORM_NAME, info_c, &platname[0], 0);
		std::cout << "Platform :" << platname << "\n";

		cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[platform_id]), 0 };
		context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
		if (context == 0) {
			std::cerr << "Can't create OpenCL context\n";
			return 0;
		}
		
		clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dev_c);
		std::vector<cl_device_id> devices(dev_c / sizeof(cl_device_id));
		clGetContextInfo(context, CL_CONTEXT_DEVICES, dev_c, &devices[0], 0);

		for (auto i = devices.begin(); i != devices.end(); i++){
			clGetDeviceInfo(*i, CL_DEVICE_NAME, 0, NULL, &info_c);
			std::string devname;
			devname.resize(info_c);
			clGetDeviceInfo(*i, CL_DEVICE_NAME, info_c, &devname[0], 0);
			std::cout << "\tDevice " << device_counter++ << ": " << devname.c_str() << "\n";
			pdpair_t pd;
			pd.device_id = i - devices.begin();
			pd.platform_id = platform_id;
			ocl_device_list.push_back(pd);
		}
		clReleaseContext(context);
	}

	if (list_available_devices) return 0;

	cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[ocl_device_list[opencl_device_id].platform_id]), 0 };
	context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
	if (context == 0) {
		std::cerr << "Can't create OpenCL context\n";
		return 0;
	}

	size_t dev_c, info_c;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dev_c);
	std::vector<cl_device_id> devices(dev_c / sizeof(cl_device_id));
	clGetContextInfo(context, CL_CONTEXT_DEVICES, dev_c, &devices[0], 0);

	device_used = devices[ocl_device_list[opencl_device_id].device_id];
	clGetDeviceInfo(device_used, CL_DEVICE_NAME, 0, NULL, &info_c);
	std::string devname;
	devname.resize(info_c);
	clGetDeviceInfo(device_used, CL_DEVICE_NAME, info_c, &devname[0], 0);
	std::cout << "Execute on Device " << opencl_device_id << ": " << devname << std::endl;
	std::cout << "OK!\n";

	queue = clCreateCommandQueue(context, device_used, 0, 0);
	if (queue == 0) {
		std::cerr << "Can't create command queue\n";
		return 0;
	}

	cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float)* iterations, NULL, NULL);
	if (cl_res == 0) {
		std::cerr << "Can't create OpenCL buffer\n";
		return 0;
	}

	FILE* f = fopen("kernel.c", "rb");
	fseek(f, 0, SEEK_END);
	size_t tell = ftell(f);
	rewind(f);
	ocl_src_char = (char*)calloc(tell + 1, 1);
	fread(ocl_src_char, tell, 1, f);

	initialized = 1;
	return 0;
}

float ocl_t::run(std::string& apl_cstr, std::string& predef){
	if (!initialized) init();
	if (list_available_devices) return -1.0f;

	cl_int err;
	auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "JIT ...\n";

	std::string source(predef);
	source.append(ocl_src_char);
    source.append("void scan_apl( rtinfo_t* rti ) {");
    source.append(apl_cstr);
    source.append("}");
    const char* cptr = source.c_str();

    cl_program program = clCreateProgramWithSource(context, 1, &cptr, 0, 0);
    if (program == 0) {
        return -1.0;
    }

	if ((err = clBuildProgram(program, 0, 0, "-cl-single-precision-constant -cl-denorms-are-zero -cl-fast-relaxed-math", 0, 0)) != CL_SUCCESS) {
		std::cerr << "Can't build program\n";
		size_t len;
		char buffer[204800];
		cl_build_status bldstatus;
		printf("\nError %d: Failed to build program executable\n", err);
		err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_STATUS, sizeof(bldstatus), (void *)&bldstatus, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Status error %d\n", err);
			exit(1);
		}
		if (bldstatus == CL_BUILD_SUCCESS) printf("Build Status: CL_BUILD_SUCCESS\n");
		if (bldstatus == CL_BUILD_NONE) printf("Build Status: CL_BUILD_NONE\n");
		if (bldstatus == CL_BUILD_ERROR) printf("Build Status: CL_BUILD_ERROR\n");
		if (bldstatus == CL_BUILD_IN_PROGRESS) printf("Build Status: CL_BUILD_IN_PROGRESS\n");
		err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), buffer, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Options error %d\n", err);
			exit(1);
		}
		printf("Build Options: %s\n", buffer);
		err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Log error %d\n", err);
			exit(1);
		}
		printf("Build Log:\n%s\n", buffer);
		exit(1);
		return -1.0;
	}
    if (program == 0) {
        std::cerr << "Can't load or build program\n";
        return -1.0;
    }

    cl_kernel sim_iterate = clCreateKernel(program, "sim_iterate", 0);
    if (sim_iterate == 0) {
        std::cerr << "Can't load kernel\n";
        clReleaseProgram(program);
        return -1.0;
    }

	float* res = new float[iterations];
	auto t2 = std::chrono::high_resolution_clock::now();

	for (auto thisstat = stat_array.begin(); thisstat != stat_array.end(); thisstat++){
		cl_uint setseed = seed ? seed : rand();
		clSetKernelArg(sim_iterate, 0, sizeof(cl_mem), &cl_res);
		clSetKernelArg(sim_iterate, 1, sizeof(cl_uint), &setseed);
		clSetKernelArg(sim_iterate, 2, sizeof(cl_uint), &thisstat->gear_str);
		clSetKernelArg(sim_iterate, 3, sizeof(cl_uint), &thisstat->gear_crit);
		clSetKernelArg(sim_iterate, 4, sizeof(cl_uint), &thisstat->gear_haste);
		clSetKernelArg(sim_iterate, 5, sizeof(cl_uint), &thisstat->gear_mastery);
		clSetKernelArg(sim_iterate, 6, sizeof(cl_uint), &thisstat->gear_mult);
		clSetKernelArg(sim_iterate, 7, sizeof(cl_uint), &thisstat->gear_vers);


		std::cout << "Sim " << thisstat->name << "...\n";
		size_t work_size = iterations;
		err = clEnqueueNDRangeKernel(queue, sim_iterate, 1, 0, &work_size, 0, 0, 0, 0);

		
		float ret, dev;

		if (err == CL_SUCCESS) {
			err = clEnqueueReadBuffer(queue, cl_res, CL_TRUE, 0, sizeof(float) * iterations, &res[0], 0, 0, 0);
			if (err != CL_SUCCESS){
				printf("Can't read back data %d\n", err);
				ret = -1.0;
			}
			else{
				ret = prefix_mean(res, iterations);
				dev = prefix_stddev(res, iterations, ret);
				std::cout << "       \r";
			}
		}
		else{
			printf("Can't run kernel %d\n", err);
			ret = -1.0;
		}

		std::cout << "Report for Stat Set " << thisstat->name << std::endl;
		std::cout << "DPS " << (dps = ret) << std::endl;
		std::cout << "DPS Range(stddev) " << dev << std::endl;
		std::cout << "DPS Error(95% conf.) " << (dpse = 2.0 * dev / sqrt(iterations)) << std::endl;
	}
    delete[] res;
    clReleaseKernel(sim_iterate);
    clReleaseProgram(program);

	
	auto t3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t1);
	std::chrono::duration<double> time_span2 = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2);
	std::cout << std::endl;
	std::cout << "Total elapsed time " << time_span1.count() << std::endl;
	std::cout << "Simulation time " << time_span2.count() << std::endl;
	std::cout << "Speedup " << (int)(iterations * max_length * stat_array.size() / time_span2.count()) << "x" << std::endl;
    return 1;
}

int ocl_t::free(){
	clReleaseMemObject(cl_res);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	return 0;
}

ocl_t& ocl(){
    static ocl_t s;
    return s;
}