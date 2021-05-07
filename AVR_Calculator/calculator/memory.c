/*
 * memory.c
 *
 * Created: 5/8/2021 12:32:19 AM
 *  Author: Dell
 */ 

#include "memory.h"

static double answer = 0;

double retrieve_last_result()
{
	return answer;
}

void save_result(double ans)
{
	answer = ans;
}
