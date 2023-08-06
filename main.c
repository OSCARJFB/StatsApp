/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

enum modes 
{

  	OPERATION,
  	MEAN,
 	MEDIAN,
  	RANGE,
  	MODE,
	ALL, 
}; 

int _operation = 0; 
int _numCount = 0; 

static double *bSortList(double *nums)
{
	for(int i = 0; i < _numCount; ++i)
	{
		for(int j = i + 1; j < _numCount; ++j)
		{
			if(nums[j] < nums[i])
			{
				double temp = nums[j]; 
				nums[j] = nums[i];
			      	nums[i] = temp;	
			}
		}
	}

	return nums; 
}

static double mean(double *nums)
{
	double sum = 0; 
	for(int i = 0; i < _numCount; ++i)
	{
		sum += nums[i]; 
	}
	return sum / (_numCount - 1);	
}	

static double median(double *nums)
{
	bSortList(nums); 
	
	int isEven = (_numCount - 1) % 2; 
	if(isEven == 0)
	{
		for(int i = 0; i < _numCount; ++i)
		{
			if(i > (float)(_numCount / 2))
			{
				return (nums[i - 1] + nums[i]) / 2; 
			}
		}
	}
	else
	{
		for(int i = 0; i < _numCount; ++i)
		{
			if(i + 1 >  (float)(_numCount / 2))
			{
				return nums[i]; 
			}
		}
	}

    	return 0;	
}	

static double range(double *nums)
{
	bSortList(nums); 
	
	int min = nums[1], max = 0;
	for(int i = 0; i < _numCount; ++i)
	{
		max = nums[i]; 
	}
	
	return max - min; 
}

static double *filterArgv(int argc, char **argv)
{
	const char *mean = "-a", *median = "-m", *range = "-r", *all = "-all";
	char *endptr;
	double *nums = NULL;
	for(int i = 0; i < argc; ++i)
	{
		if(strcmp(*(argv + i), mean) == 0)
		{
			_operation = MEAN;
			break; 
		}
		else if(strcmp(*(argv + i), median) == 0)
		{
			_operation = MEDIAN;
			break;
		}
		else if(strcmp(*(argv + i), range) == 0)
		{
			_operation = RANGE;
			break;
		}
		else if(strcmp(*(argv + i), all) == 0)
		{
			_operation = ALL;
			break;
		}

		else
		{
			double result = strtod(*(argv + i), &endptr);
			if(errno != 0)
			{
				perror("Error invalid numerical input.");
				exit(1);
			}
			nums = realloc(nums, sizeof(double) * (i + 1));
			if(nums == NULL)
			{
				perror("Error allocating memory.");
				exit(1); 
			}
			nums[i] = result;
		       	++_numCount; 	
		}
	}

	if(_operation == 0 || _numCount == 0) 	
	{	
		printf("Invalid arguments supplied"); 
		exit(1); 
	}

	return nums;
}

static void switchAndPrint(double *nums)
{
	switch(_operation)
	{
		case MEAN:	
			printf("Mean(average): %f", mean(nums)); 
			break;
		case MEDIAN:
			printf("Median: %f", median(nums));
			break;
		case RANGE:
			printf("Range: %f", range(nums));
			break;
		case ALL:
			printf("Mean: %f\n", mean(nums));		
			printf("Median: %f\n", median(nums));
			printf("Range: %f\n", range(nums));
			break;
	}
}

int main(int argc, char **argv)
{
	
	double *nums = filterArgv(argc, argv); 
	switchAndPrint(nums);
        free(nums);
	nums = NULL; 	
    	return 0;	
}
