/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

enum modes 
{

  	OPERATION,
  	MEAN,
 	MEDIAN,
  	RANGE,
  	MODE,
	STD_D, 
	ALL, 
}; 

int _operation = 0; 
int _numCount = 0; 

typedef struct numCount
{
	double num;
	int count; 
	struct numCount *next; 
} numCount; 

/**
 * Assert any memory allocation using this function.
 * We just exit if the kernel don't accept our request for memory. 
 */
void *allocateMem(void *memory)
{
	if(memory == NULL) 
	{
		perror("Critical error memory allocation failed. Exit with return code 1");
	        exit(1);	
	}

	return memory;
}

/**
 * Fetch the result from the hashset. 
 * Allocate and store the value of each mode(s).
 */
static int getResultsFromHash(numCount *headNode, double **results)
{
	double *result = NULL; 
	int maxCount = 0, modes = 0;	
	for(numCount *node = headNode; node->next != NULL; node = node->next)
	{
		if(node->count > maxCount)
	{
			maxCount = node->count;
		}
	}

	for(numCount *node = headNode; node->next != NULL; node = node->next)
	{
		if(node->count == maxCount)
		{
			result = allocateMem(realloc(result, sizeof(double) * (modes + 1))); 
			result[modes++] = node->num;
		}
	}

	*results = result;
	return modes;
}

/**
 * Set the hashset
 * if a value exists in the set increment its count by one. 
 */
static void setHash(double num, numCount **headNode)
{
	if(*headNode == NULL)
	{
		numCount *newNode = allocateMem(malloc(sizeof(numCount)));  
		newNode->num = num; 
		newNode->next = NULL; 	
		newNode->count += 1; 
		*headNode = newNode; 
	       	return; 	
	}

	numCount *node = *headNode; 
	while(node != NULL)
	{
		if(node->num == num)
		{
			node->count += 1; 
			return; 
		}

		if(node->next == NULL)
		{
			break; 
		}
		node = node->next;
	}


	numCount *newNode = allocateMem(malloc(sizeof(numCount))); 
	newNode->num = num; 
	newNode->next = NULL; 	
	newNode->count += 1; 
	node->next = newNode;  
}


/** 
 * No need for the hashset.
 * Delete/free each node in the set. 
 */
static void deleteHash(numCount **headNode)
{
	while(*headNode != NULL)
	{
		numCount *temp = *headNode; 
		*headNode = (*headNode)->next; 
		free(temp); 
		temp = NULL; 
	}
}

/** 
 * A bubblesort just doing its job. 
 * Sorted arrays are need for some of the statistic operations or make them easier to calculate.
 */
static void bSortList(double *nums)
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
}


/**
 * How to find the mean: 
 * Sum all values in the data set. 
 * Finally divide the sum with total amount values in the set. 
 */
static double mean(double *s)
{
	double sum = 0; 
	for(int i = 0; i < _numCount; ++i)
	{
		sum += s[i]; 
	}

	printf("Mean %f\n", sum / (_numCount - 1));
	return sum / (_numCount - 1); 	
}	

/**
 * How to find the median: 
 * Our data set needs to be sorted.  
 * The median in an uneven dataset is the middle value. 
 * If the data set is even the two values in the middle summerized and the diveded by two will be the median. 
 */
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
				printf("Median: %f\n", (nums[i - 1] + nums[i]) / 2);
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
				printf("Median: %f\n", nums[i]);
				return nums[i]; 
			}
		}
	}

	printf("Median: %f\n", 0.f);
    	return 0;	
}	

/**
 * How to find the range.
 * Subtract min from max value.
 */
static double range(double *nums)
{
	bSortList(nums); 
	double min = nums[0], max = 0;
	for(int i = 0; i < _numCount; ++i)
	{
		max = nums[i]; 
	}
	
	printf("Range: %f\n", max - min);
	return max - min; 
}


/**
 * How to find the mode(s)
 * Modes are the most frequent numbers in a data set.
 * There can be several modes so long as they acount for the maximum frequency in the set.
 * To find this we sort the data set, then add each unique value to hashset and add a count to each time this value is found.
 * The values being of highest frequency gets printed. 
 */
static double *mode(double *nums)
{
	numCount *headNode = NULL;  
	bSortList(nums);
	for(int i = 1; i <= _numCount; ++i)
	{	
		setHash(nums[i], &headNode);
	}

	double *result = NULL;
	int modes = getResultsFromHash(headNode, &result);
	if(result == NULL)
	{	
		return 0; 
	}	

	printf("Modes: "); 
	for(int i = 0; i < modes; ++i)
	{
		printf("%f ", result[i]); 
	}
	puts("");

	free(result); 	
	result = NULL; 
	deleteHash(&headNode);
}

/**
 *  Calculate the standard deviation
 *  (m) is the mean and (d) is the deviation of each data point.
 *  (s) the sum of squares of all deviations (v) the variance.
 *  (n) represents the samples and (std_d) the result. 
 *
 *
 *  TODO: right now we calculate only population we shoud also enable sample using v = (s / n - 1) 
 *  To enable this we should have an arugment telling the function which type we want. 
 */
static double stdDev(double *nums)
{
	double m = 0, *d = NULL, v = 0, std_d = 0, s = 0, ms = 0;
	int n = 0; 

	for(int i = 0; i < _numCount; ++i)
	{
		ms += nums[i]; 
	}

	m = ms / (_numCount - 1); 		// Step one: Get the mean of the data set.
	for(int i = 1; i < _numCount; ++i)
	{
		d = allocateMem(realloc(d, sizeof(double) * (i + 1)));
		d[i] = nums[i] - m;		// Step two: Find out how each data point deviates from the mean.
		d[i] = d[i] * d[i];		// Step three: Square all of the found deviations. 
		s += d[i]; 			// Step four: Sum all the squares.
		++n;
	}
	v = s / n;				// Step five: find the variance.
	std_d = sqrt(v);			// Step six: Get the square root of the variance.
	
	free(d);
	d = NULL;

	printf("Standard deviation: %f\n", std_d);
	return std_d; 
}

/**
 * Just check the args.
 * Find out the operation type and the actual data.
 */
static double *filterArgv(int argc, char **argv)
{
	const char *mean = "-a", *median = "-m", *range = "-r", *mode = "-o", *all = "-all", *std_d = "-std";
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
		else if(strcmp(*(argv + i), mode) == 0)
		{
			_operation = MODE;
			break;
		}
		else if(strcmp(*(argv + i), std_d) == 0)
		{
			_operation = STD_D;
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
			nums = allocateMem(realloc(nums, sizeof(double) * (i + 1)));
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

/**
 * Execute the operations.
 * This is dependant on the input of args.
 */
static void switchAndPrint(double *s)
{
	switch(_operation)
	{
		case MEAN:	
			(void)mean(s);
			break;
		case MEDIAN:
			(void)median(s);
			break;
		case RANGE:
			(void)range(s);
			break;
		case MODE:
			(void)mode(s);
			break;
		case STD_D:
			(void)stdDev(s); 
			break;
		case ALL:
			(void)mean(s);
			(void)median(s);
			(void)range(s);
			(void)mode(s);
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
