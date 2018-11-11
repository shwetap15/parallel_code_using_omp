#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif



int main (int argc, char* argv[]) {
	//forces openmp to create the threads beforehand
	#pragma omp parallel
	{
		int fd = open (argv[0], O_RDONLY);
		if (fd != -1) {
			close (fd);
		}
		else {
			std::cerr<<"something is amiss"<<std::endl;
		}
	}

	if (argc < 9) {
		std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
		return -1;
	}

	//insert code here
	int function_id = atoi(argv[1]);
	float a = atof(argv[2]);
	float b = atof(argv[3]);
	int n = atoi(argv[4]);
	int intensity = atoi(argv[5]);
	int nbthreads = atoi(argv[6]);
	std::string scheduling = argv[7];
	long granularity = atoi(argv[8]);
	
	float ba_by_n = ((b-a)/n);
	float output = 0;
	
	omp_set_num_threads(nbthreads);
	
	//set omp scheduling method
	if(scheduling.compare("static") == 0)
	{
		omp_set_schedule(omp_sched_static,granularity);
	}
	else if(scheduling.compare("dynamic") == 0)
	{
		omp_set_schedule(omp_sched_dynamic , granularity);
	}
	else if(scheduling.compare("guided") == 0)
	{
		omp_set_schedule(omp_sched_guided , granularity);
	}
	
	omp_lock_t mut;
	omp_init_lock(&mut);
	
	// start timing
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	
	#pragma omp parallel for schedule(runtime)
	for(long i = 0;i<n;i++)
	{
		float x = 0;
		float j = 0;
		j = (i+0.5)*ba_by_n;
		x = a+j;
		
		omp_set_lock(&mut);
		switch(function_id)
		{
			case 1 : {
					output += f1(x,intensity);
					break;
				}
			case 2 : {
					output += f2(x,intensity);
					break;
				}
			case 3 : {
					output += f3(x,intensity);
					break;
				}
			case 4 : {
					output += f4(x,intensity);
					break;
				}
			default:break;		
		}	
		omp_unset_lock(&mut);
	}
	
	output = output*ba_by_n;

	omp_destroy_lock(&mut);
	
	// end time
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapased_seconds = end-start;

	// display result
	std::cout<<output;
	std::cerr<<elapased_seconds.count();
	
	return 0;
}
