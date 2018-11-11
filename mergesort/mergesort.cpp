#include <omp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
	#endif
	void generateMergeSortData (int* arr, size_t n);
	void checkMergeSortResult (int* arr, size_t n);
	#ifdef __cplusplus
}
#endif


void merge(int * arr, int * temp, int start, int mid, int end, int n)
{
	if (start == end) return;
	
	int k = start, i = start, j = mid + 1;
	
	while (i <= mid && j <= end)
	{
		if (arr[i] < arr[j])
			temp[k++] = arr[i++];
		else
			temp[k++] = arr[j++];
	}
	
	while (i < n && i <= mid)
		temp[k++] = arr[i++];
	
	
	#pragma omp parallel for 
	for (int i = start; i <= end; i++)
	{	
		arr[i] = temp[i];
	}	
		
}


void mergesort(int* arr, int * temp, int left, int right, int n) 
{
	for (int i = 1; i <= right - left; i = 2*i)
	{
		//#pragma omp parallel for schedule(static)
		#pragma omp parallel for 
		for (int j = left; j < right; j += 2*i)
		{
			int start = j;
			int mid = j + i - 1;
			int end = std::min(start + 2*i - 1, right);

			merge(arr, temp, start, mid, end,n );
		}
	}
}

int main (int argc, char* argv[]) 
{
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

	if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
		return -1;
	}

	int nbthreads = atoi(argv[2]);
	omp_set_num_threads(nbthreads);

	int n = atoi(argv[1]);
	
	int * arr = new int [n];
	int * temp = new int [n];
	
	// get arr data
	generateMergeSortData (arr, n);

	// generate random input of integers
	for (int i = 0; i < n; i++)
		temp[i] = arr[i] ;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	//insert sorting code here.
	
	mergesort(arr, temp, 0, n - 1,n);
	
	
	/*#pragma omp parallel
	{
		#pragma omp single
		mergesort(arr, temp, 0, n - 1,n);
	}	*/
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::cerr<<elapsed_seconds.count()<<std::endl;

	checkMergeSortResult (arr, n);

	delete[] arr;

	return 0;
}
