/*
	climb: aean's stats optimizer.
	2015.1.30, Aean

	IreCore is distributed under the terms of The MIT License.
    You should have received a copy of the MIT License along with this program.
    If not, see <http://opensource.org/licenses/mit-license.php>.
*/

#include "irecore.h"
#include <set>
#include <functional>

const char* stat_name[] = {
	"crit",
	"haste",
	"mastery",
	"multistrike",
	"versatility",
};
const int stat_count = sizeof stat_name / sizeof *stat_name;

struct point_t{
	int stat[5];
	point_t(int amount){
		stat[4] = amount;
		for (int i = 0; i < stat_count - 1; i++){
			stat[i] = amount / stat_count;
			stat[4] -= stat[i];
		}
	}
	int go(int direct1, int direct2, int interval = 1){
		if (direct1 != direct2 && direct1 >= 0 && direct1 < stat_count && direct2 >= 0 && direct2 < stat_count){
			stat[direct1] += interval;
			stat[direct2] -= interval;
		} else return 0;
		for (int i = 0; i < stat_count; i++){
			if (stat[i] < 0) return 0;
		}
		return 1;
	}

};

namespace std {

	template <>
	struct hash<point_t>
	{
		std::size_t operator()(const point_t& k) const
		{
			using std::size_t;
			using std::hash;
			size_t h = 0;
			
			for (int i = 0; i < stat_count; i++){
				h += k.stat[i];
				h = hash<int>()(h);
			}

			return h;
		}
	};

}

struct simresult_t{
	double dps;
	double error;
	int iter;
	point_t point;
	simresult_t(point_t p, double d, double e, int i) :
		point(p), dps(d), error(e), iter(i) {}
};

std::set<size_t> tt;
int transpose(point_t point){
	std::hash<point_t> h;
	int c = tt.count(h(point));
	tt.insert(h(point));
	return c;
}

simresult_t sim(point_t& point){
	int iter = 8192;
	char digits[16];
	//current_stat = stat_array[0];
	stat_array.clear();
	current_stat.gear_crit = point.stat[0];
	current_stat.gear_haste = point.stat[1];
	current_stat.gear_mastery = point.stat[2];
	current_stat.gear_mult = point.stat[3];
	current_stat.gear_vers = point.stat[4];
	current_stat.name = "climb";
	std::hash<point_t> h;
	current_stat.name.append(ultoa(h(point), digits, 16));
	stat_array.push_back(current_stat);
	parameters_consistency();

	double dps, error;
	iterations = iter;
	ocl().run(apl, predef, 1);
	dps = stat_array[0].dps;
	error = stat_array[0].dpse / 2.0;
	return simresult_t(point, dps, error, iter);
}

const simresult_t& sim(simresult_t& r, double target_error){
	int iter = r.iter;
	double error = r.error;
	double dps = r.dps;
	point_t point = r.point;
	if (error * 2.0 <= target_error) return r;
	char digits[16];
	//current_stat = stat_array[0];
	stat_array.clear();
	current_stat.gear_crit = point.stat[0];
	current_stat.gear_haste = point.stat[1];
	current_stat.gear_mastery = point.stat[2];
	current_stat.gear_mult = point.stat[3];
	current_stat.gear_vers = point.stat[4];
	current_stat.name = "xclimb";
	std::hash<point_t> h;
	current_stat.name.append(ultoa(h(point), digits, 16));
	stat_array.push_back(current_stat);
	parameters_consistency();


	if (error * 2.0 > target_error){
		int more_iter = 1024 + static_cast<int>(iter * (error * 2.0 / target_error) * (error * 2.0 / target_error));
		more_iter -= iter;
		if (more_iter > 0) {
			if (more_iter > 100000) more_iter = 100000;
			double more_dps, more_error;
			iterations = more_iter;
			ocl().run(apl, predef, 1);
			more_dps = stat_array[0].dps;
			more_error = stat_array[0].dpse / 2.0;
			dps = (dps * iter) + (more_dps * more_iter);
			dps /= more_iter + iter;
			iter += more_iter;
			more_error = more_error * more_error * more_iter * more_iter;
			error = more_error + error * error * iter * iter;
			error /= more_iter + iter;
			error /= more_iter + iter;
			error = sqrt(error);
		}
	}
	r.iter = iter;
	r.dps = dps;
	r.error = error;
	return r;
};

void descent(int init_interval, int min_interval, int iteration_limit){
	point_t p(current_stat.gear_crit + current_stat.gear_haste + current_stat.gear_mastery + current_stat.gear_mult + current_stat.gear_vers);
	simresult_t res = sim(p);
	tt.clear();
	transpose(p);
	int interval = init_interval;
	while (interval >= min_interval){
		*report_path << "interval set to " << interval << std::endl;
		int found, reserved, confident;
		simresult_t reserved_res = res;
		do{
			found = 0;
			reserved = 0;
			confident = 1;
			for (int i = 0; i < stat_count; i++){
				for (int j = 0; j < stat_count; j++){
					point_t test(p);
					if (!test.go(i, j, interval))
						continue;
					if (transpose(test))
						continue;
					*report_path << "test +" << interval << stat_name[i] << " -" << interval << stat_name[j] << std::endl;
					simresult_t testres = sim(test);
					*report_path << "\tdps " << testres.dps << " " << res.dps << ", sigma " << testres.error << " " << res.error << ", delta " << abs(testres.dps - res.dps) << std::endl;
					while (1.47 * (testres.error + res.error) > abs(testres.dps - res.dps)){
						if (testres.iter + res.iter >= iteration_limit) {
							if (!reserved && testres.dps >= res.dps){
								*report_path << "\ttest iteration exceeded (" << testres.iter << "), reserved" << std::endl;
								reserved = 1;
								reserved_res = testres;
								confident = 0;
								break;
							}
							else{
								*report_path << "\ttest iteration exceeded (" << testres.iter << "), but not reserved" << std::endl;
								confident = 0;
								break;
							}
						}
						if (res.error >= 0.8 * testres.error) sim(res, res.error * 1.8 * (abs(testres.dps - res.dps) / (1.47 * (testres.error + res.error))));
						else sim(testres, testres.error * 1.8 * (abs(testres.dps - res.dps) / (1.47 * (testres.error + res.error))));
						*report_path << "\tdps " << testres.dps << " " << res.dps << ", sigma " << testres.error << " " << res.error << ", delta " << abs(testres.dps - res.dps) << std::endl;
					}
					if (confident && testres.dps > res.dps){
						res = testres;
						p = test;
						found = 1;
						*report_path << "go +" << stat_name[i] << " -" << stat_name[j] << ", dps " << res.dps << ", now at(";
						for (int j = 0; j < stat_count; j++)
							*report_path << p.stat[j] << ((j == stat_count - 1) ? "" : ",");
						*report_path << ")" << std::endl;
						break;
					}
					confident = 1;
				}
				if (found) break;
			}
			if (!found && reserved){
				p = reserved_res.point;
				res = reserved_res;
				*report_path << "go reserved, dps " << res.dps << ", now at(";
				for (int j = 0; j < stat_count; j++)
					*report_path << p.stat[j] << ((j == stat_count - 1) ? "" : ",");
				*report_path << ")" << std::endl;
				found = 1;
			}
		} while (found); 
		interval = static_cast<int>(interval * 0.618);
	}
	*report_path << "** maxima confirmed at(";
	for (int j = 0; j < stat_count; j++)
		*report_path << p.stat[j] << ((j == stat_count - 1) ? "" : ",");
	*report_path << ") **" << std::endl;
}

#define MSK_CRIT (1 << 0)
#define MSK_HASTE (1 << 1)
#define MSK_MASTERY (1 << 2)
#define MSK_MULT (1 << 3)
#define MSK_VERS (1 << 4)

void plot(unsigned mask, int interval, double error_tolerance, int iteration_limit){
	FILE* f = fopen("plot.sci", "wb");
	tt.clear();
	point_t p(0);
	int amount = (mask & MSK_CRIT ? current_stat.gear_crit : 0) +
		(mask & MSK_HASTE ? current_stat.gear_haste : 0) +
		(mask & MSK_MASTERY ? current_stat.gear_mastery : 0) +
		(mask & MSK_MULT ? current_stat.gear_mult : 0) +
		(mask & MSK_VERS ? stat_array[0].gear_vers : 0);
	p.stat[0] = current_stat.gear_crit;
	p.stat[1] = current_stat.gear_haste;
	p.stat[2] = current_stat.gear_mastery;
	p.stat[3] = current_stat.gear_mult;
	p.stat[4] = current_stat.gear_vers;

	unsigned long msidx[3];
	_BitScanForward(&msidx[0], mask);
	mask &= mask - 1;
	_BitScanForward(&msidx[1], mask);
	mask &= mask - 1;
	_BitScanForward(&msidx[2], mask);
	fprintf(f, "raw=[");
	simresult_t res = sim(p);
	int data_count = 0;
	int axis_max = 0;
	for (p.stat[msidx[0]] = 0; p.stat[msidx[0]] <= amount; p.stat[msidx[0]] += interval){
		for (p.stat[msidx[1]] = 0; p.stat[msidx[1]] <= amount; p.stat[msidx[1]] += interval){
			p.stat[msidx[2]] = amount - p.stat[msidx[1]] - p.stat[msidx[0]];
			if (p.stat[msidx[2]] > amount || p.stat[msidx[2]] < 0) continue;
			res = sim(p);
			while (res.error * 2 > error_tolerance){
				res = sim(res, error_tolerance);
				if (res.iter > iteration_limit) break;
			}
			fprintf(f, "%d, %d, %.3lf, %.3lf\r\n", p.stat[msidx[0]], p.stat[msidx[1]], res.dps, res.error);
			fflush(f);
			data_count++;
			*report_path << p.stat[msidx[0]] << stat_name[msidx[0]] << ", " << p.stat[msidx[1]] << stat_name[msidx[1]] << ", " << p.stat[msidx[2]] << stat_name[msidx[2]] << ", dps:" << res.dps << ", error:" << res.error * 2 << std::endl;
		}
		axis_max = p.stat[msidx[0]];
	}
	fprintf(f, "];\r\n");
	fprintf(f, "for i=1:%d z(1+(raw(i,1)/%d),1+(raw(i,2)/%d))=raw(i,3); end\r\n", data_count, interval, interval);
	fprintf(f, "rawmin=raw(1,3);\r\n");
	fprintf(f, "rawmax=raw(1,3);\r\n");
	fprintf(f, "for i=1:%d if rawmin>raw(i,3) then rawmin=raw(i,3); end end\r\n", data_count);
	fprintf(f, "for i=1:%d if rawmax<raw(i,3) then rawmax=raw(i,3); end end\r\n", data_count);
	fprintf(f, "nz=linspace(rawmin,rawmax,30);\r\n");
	fprintf(f, "%s=linspace(%d,%d,%d);\r\n", stat_name[msidx[0]], 0, axis_max, 1 + axis_max / interval);
	fprintf(f, "%s=%s;\r\n", stat_name[msidx[1]], stat_name[msidx[0]]);
	fprintf(f, "contourf(%s, %s, z, nz);\r\n", stat_name[msidx[0]], stat_name[msidx[1]]);
	fprintf(f, "f=gcf();\r\n"
		"f.color_map=bonecolormap(30);\r\n"
		"xlabel(\"%s\");\r\n"
		"ylabel(\"%s\");\r\n", stat_name[msidx[0]], stat_name[msidx[1]]);
	fprintf(f, "axc=gca();\r\n"
		"axc.auto_ticks=\"on\";\r\n");
	fclose(f);
}
