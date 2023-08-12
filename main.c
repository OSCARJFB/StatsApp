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

typedef struct numCount
{
	double num;
	int count; 
	struct numCount *next; 
} numCount; 

void *allocateMem(void *memory)
{
	if(memory == NULL) 
	{
		perror("Critical error memory allocation failed. Exit with return code 1");
	        exit(1);	
	}

	return memory;
}

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

static void setHash(double num, numCount **headNode)
{
	if(*headNode == NULL)
	{
		numCount *newNode = allocateMemory(malloc(sizeof(numCount)));  
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
 * A sorted array is need for some of the statistic operations.
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

// Calculate standard deviation
static double stdDev(double *nums)
{
	// Step one: Get the mean of the data set.
	double m = mean(nums); 

	// Step two: Find out how each data point deviates from the mean.
	// Step three: Square all of the found deviations. 
	double *dev = NULL; 
	for(int i = 0; i <= _numsCount; ++i)
	{
		dev = allocateMem(realloc(dev, sizeof(double) * i + 1));
		dev[i] = nums[i] - m;	
		dev[i] = dev[i] * dev[i];
	}

}

static double *filterArgv(int argc, char **argv)
{
	const char *mean = "-a", *median = "-m", *range = "-r", *mode = "-o", *all = "-all";
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
