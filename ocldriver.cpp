#include "irecore.h"

char* ocl_src_char;

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
	cl_int err;
	cl_uint num;
	std::vector<cl_platform_id> platforms;


	*report_path << "Query available compute devices ..." << std::endl;

	err = clGetPlatformIDs(0, 0, &num);
	if (err != CL_SUCCESS) {
		*report_path << "Unable to get platforms" << std::endl;
		return 0;
	}

	platforms.resize(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);
	if (err != CL_SUCCESS) {
		*report_path << "Unable to get platform ID" << std::endl;
		return 0;
	}
	if (ocl_device_list.empty()){
		int device_counter = 0;
		for (size_t platform_id = 0; platform_id < num; platform_id++){
			size_t dev_c, info_c;
			clGetPlatformInfo(platforms[platform_id], CL_PLATFORM_NAME, 0, NULL, &info_c);
			std::string platname;
			platname.resize(info_c);
			clGetPlatformInfo(platforms[platform_id], CL_PLATFORM_NAME, info_c, &platname[0], 0);
			*report_path << "Platform :" << platname << std::endl;

			cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[platform_id]), 0 };
			context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
			if (context == 0) {
				*report_path << "Can't create OpenCL context" << std::endl;
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
				*report_path << "\tDevice " << device_counter++ << ": " << devname.c_str() << std::endl;
				pdpair_t pd;
				pd.device_id = i - devices.begin();
				pd.platform_id = platform_id;
				ocl_device_list.push_back(pd);
			}
			clReleaseContext(context);
		}
	}
	if (list_available_devices) return 0;

	cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[ocl_device_list[opencl_device_id].platform_id]), 0 };
	context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
	if (context == 0) {
		*report_path << "Can't create OpenCL context" << std::endl;
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
	*report_path << "Execute on Device " << opencl_device_id << ": " << devname << std::endl;
	*report_path << "OK!" << std::endl;

	queue = clCreateCommandQueue(context, device_used, 0, 0);
	if (queue == 0) {
		*report_path << "Can't create command queue" << std::endl;
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

float ocl_t::run(std::string& apl_cstr, std::string& predef, int reuse){
	if (!initialized) init();
	if (list_available_devices) return -1.0f;

	cl_int err;
	auto t1 = std::chrono::high_resolution_clock::now();
	cl_program program = 0;

	if (!reuse || !program_reuse){
		*report_path << "JIT ..." << std::endl;

		std::string source(predef);
		source.append(ocl_src_char);
		source.append("void scan_apl( rtinfo_t* rti ) {");
		source.append(apl_cstr);
		source.append("}");
		const char* cptr = source.c_str();

		if (program_reuse){
			clReleaseProgram(program_reuse);
			program_reuse = 0;
		}
		program_reuse = program = clCreateProgramWithSource(context, 1, &cptr, 0, 0);
		if (program == 0) {
			return -1.0;
		}

		if ((err = clBuildProgram(program, 1, &device_used, "-cl-single-precision-constant -cl-denorms-are-zero -cl-fast-relaxed-math", 0, 0)) != CL_SUCCESS) {
			*report_path << "Can't build program" << std::endl;
			size_t len;
			char buffer[204800];
			cl_build_status bldstatus;
			*report_path << "\nError " << err << ": Failed to build program executable" << std::endl;
			err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_STATUS, sizeof(bldstatus), (void *)&bldstatus, &len);
			if (err != CL_SUCCESS)
			{
				*report_path << "Build Status error " << err << std::endl;
				return -1.0;
			}
			if (bldstatus == CL_BUILD_SUCCESS) *report_path << "Build Status: CL_BUILD_SUCCESS" << std::endl;
			if (bldstatus == CL_BUILD_NONE) *report_path << "Build Status: CL_BUILD_NONE" << std::endl;
			if (bldstatus == CL_BUILD_ERROR) *report_path << "Build Status: CL_BUILD_ERROR" << std::endl;
			if (bldstatus == CL_BUILD_IN_PROGRESS) *report_path << "Build Status: CL_BUILD_IN_PROGRESS" << std::endl;
			err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), buffer, &len);
			if (err != CL_SUCCESS)
			{
				*report_path << "Build Options error " << err << std::endl;
				return -1.0;
			}
			*report_path << "Build Options: " << buffer << std::endl;
			err = clGetProgramBuildInfo(program, device_used, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
			if (err != CL_SUCCESS)
			{
				*report_path << "Build Log error " << err << std::endl;
				return -1.0;
			}
			*report_path << "Build Log:\n" << buffer << std::endl;
			return -1.0;
		}
	}
	else{
		program = program_reuse;
	}

	if (program == 0) {
		*report_path << "Can't load or build program" << std::endl;
		return -1.0;
	}

    cl_kernel sim_iterate = clCreateKernel(program, "sim_iterate", 0);
    if (sim_iterate == 0) {
		*report_path << "Can't load kernel" << std::endl;
        clReleaseProgram(program);
        return -1.0;
    }

	cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float)* iterations, NULL, NULL);
	if (cl_res == 0) {
		*report_path << "Can't create OpenCL buffer" << std::endl;
		return 0;
	}
	float* res = new float[iterations];
	auto t2 = std::chrono::high_resolution_clock::now();

	for (auto thisstat = stat_array.begin(); thisstat != stat_array.end(); thisstat++){
		cl_uint setseed = (seed && !reuse) ? seed : rand();
		clSetKernelArg(sim_iterate, 0, sizeof(cl_mem), &cl_res);
		clSetKernelArg(sim_iterate, 1, sizeof(cl_uint), &setseed);
		clSetKernelArg(sim_iterate, 2, sizeof(cl_uint), &thisstat->gear_str);
		clSetKernelArg(sim_iterate, 3, sizeof(cl_uint), &thisstat->gear_crit);
		clSetKernelArg(sim_iterate, 4, sizeof(cl_uint), &thisstat->gear_haste);
		clSetKernelArg(sim_iterate, 5, sizeof(cl_uint), &thisstat->gear_mastery);
		clSetKernelArg(sim_iterate, 6, sizeof(cl_uint), &thisstat->gear_mult);
		clSetKernelArg(sim_iterate, 7, sizeof(cl_uint), &thisstat->gear_vers);

		if (!reuse)
			*report_path << "Sim " << thisstat->name << "..." << std::endl;
		size_t work_size = iterations;
		err = clEnqueueNDRangeKernel(queue, sim_iterate, 1, 0, &work_size, 0, 0, 0, 0);

		
		float ret, dev;

		if (err == CL_SUCCESS) {
			err = clEnqueueReadBuffer(queue, cl_res, CL_TRUE, 0, sizeof(float) * iterations, &res[0], 0, 0, 0);
			if (err != CL_SUCCESS){
				*report_path << "Can't read back data " << err << std::endl;
				ret = -1.0;
			}
			else{
				ret = prefix_mean(res, iterations);
				dev = prefix_stddev(res, iterations, ret);
				std::cout << "       \r";
			}
		}
		else{
			*report_path << "Can't run kernel " << err << std::endl;
			ret = -1.0;
		}
		
		thisstat->dps = ret;
		thisstat->dpsr = dev;
		thisstat->dpse = 2.0 * dev / sqrt(iterations);

		if (!reuse){
			*report_path << "Report for Stat Set " << thisstat->name << std::endl;
			*report_path << "DPS " << thisstat->dps << std::endl;
			*report_path << "DPS Range(stddev) " << thisstat->dpsr << std::endl;
			*report_path << "DPS Error(95% conf.) " << thisstat->dpse << std::endl;
		}
	}
    delete[] res;
    clReleaseKernel(sim_iterate);
	clReleaseMemObject(cl_res);
    //clReleaseProgram(program);

	
	auto t3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t1);
	std::chrono::duration<double> time_span2 = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2);
	if (!reuse){
		*report_path << std::endl;
		*report_path << "Total elapsed time " << time_span1.count() << std::endl;
		*report_path << "Simulation time " << time_span2.count() << std::endl;
		*report_path << "Speedup " << (int)(iterations * max_length * stat_array.size() / time_span2.count()) << "x" << std::endl;
		*report_path << std::endl;
	}
    return 1;
}

int ocl_t::free(){
	if (program_reuse){
		clReleaseProgram(program_reuse);
		program_reuse = 0;
	}
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	initialized = 0;
	return 0;
}

ocl_t& ocl(){
    static ocl_t s;
    return s;
}