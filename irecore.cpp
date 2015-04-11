#include "irecore.h"

stat_t stat;
raidbuff_t raidbuff;
cl_uint seed;
std::string apl;
std::string predef;
int iterations;

void err(const char* format, ...){
	va_list vl;
	va_start(vl, format);
	vprintf(format, vl);
	va_end(vl);
	exit(-1);
}

void set_default_parameters(){
	stat = {
		3945, 1714, 917, 1282, 478, 0,
	};
	raidbuff = {
		1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
	seed = (cl_uint)time(NULL);
	apl = "SPELL(bloodthirst); SPELL(execute); SPELL(ragingblow); SPELL(wildstrike);";
	iterations = 100000;
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
		}
		else if (0 == i->key.compare("gear_crit")){
			stat.gear_crit = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("gear_mastery")){
			stat.gear_mastery = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("gear_haste")){
			stat.gear_haste = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("gear_mult")){
			stat.gear_mult = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("gear_vers")){
			stat.gear_vers = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("deterministic_seed")){
			seed = atoi(i->value.c_str());
		}
		else if (0 == i->key.compare("iterations")){
			iterations = atoi(i->value.c_str());
			if (iterations <= 0) iterations = 1;
		}
	}
}

int main(int argc, char** argv){
	set_default_parameters();
	std::vector<kvpair_t> arglist;
	build_arglist(arglist, argc, argv);
	parse_parameters(arglist);
	std::cout << ocl().run(apl) << std::endl;
//	host_kernel_entry();
	return 0;
}