
#ifndef YIJINJING_TEST_STAT_H
#define YIJINJING_TEST_STAT_H

#include <stdlib.h>

void cpu_set_affinity(int cpu_id);


class Calculator
{
public:
	static void print_header() ;
	static void print_footer() ;
private:
    int n;
    int64_t min;
    int64_t max;
    int64_t sum;
    int64_t square_sum;
public:
    Calculator();
    void update(int64_t d);
   
    void print() const;
};


#endif