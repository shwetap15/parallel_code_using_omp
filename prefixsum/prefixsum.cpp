#include <chrono>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
  
  
#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
		return -1;
	}

	int n = atoi(argv[1]);

	int nbthreads = atoi(argv[2]);
	omp_set_num_threads(nbthreads);

	int * arr = new int [n];
	int* prefix = new int [n+1];
	
	
	generatePrefixSumData (arr, n);

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	prefix[0] = arr[0]; 
	int *partial_sum;
	#pragma omp parallel
	{
		int thread_id = omp_get_thread_num();
		
		#pragma omp single
		{
			partial_sum = new int[nbthreads+1];
			partial_sum[0] = 0;
		}
		int sum = 0;
		#pragma omp for schedule(auto) nowait 
		for(int i=0; i<n; i++) {
			sum += arr[i];
			prefix[i+1] = sum;
		}
		partial_sum[thread_id+1] = sum;
		
		#pragma omp barrier
		int x = 0;
		for(int i=0; i<(thread_id+1); i++) {
			x += partial_sum[i];
		}

		#pragma omp for schedule(auto) 
		for(int i=0; i<n; i++) {
			prefix[i+1] += x;
		}
	} 
	
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::cerr<<elapsed_seconds.count()<<std::endl;

	checkPrefixSumResult(prefix, n);

	delete[] arr;
	delete[] prefix;
	delete[] partial_sum;

	return 0;
}
