
#include "stat.h"
#include <math.h>
#if defined __linux__  
#include <sched.h>
#endif
#include <iostream>
#include <stdio.h>


void cpu_set_affinity(int cpu_id){
#if defined __linux__  
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu_id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        std::cout<<"set cpu affinity failed, cpu_id " << cpu_id << std::endl;
    }
    else{
        std::cout<<"set cpu affinity success, cpu_id " << cpu_id << std::endl;
    }
#endif    
}

void Calculator::print_header() {
	std::cout << "+-------------+--------------+--------------+--------------+-------+" << std::endl;
	std::cout << " mean(ns)     | std(ns)      | min(ns)      | max(ns)      | valid |" << std::endl;
	std::cout << "--------------+--------------+--------------+--------------+-------+" << std::endl;		
	}
void Calculator::print_footer() {
  std::cout << "+--------------+--------------+--------------+--------------+-------+" << std::endl;		
	}

Calculator::Calculator(): n(0), min(1000000000), max(0), sum(0), square_sum(0) {}
void Calculator::update(int64_t d)
{
    ++n;
    sum += d;
    square_sum += (int64_t)d*d;
    ///min = (min < 0) ? d : ((d < min) ? d : min);
//        min = (d < min) ? d : min;
//        max = (d > max) ? d : max;
    if(d<min) min = d;
    if(d>max) max = d;
}

void Calculator::print() const
{
    printf("%14ld|%14.5e|%14ld|%14ld|%7d|\n",
           (n == 0)? 0: sum / n,
           (n == 0)? 0: sqrt((square_sum - (double)sum * sum / n) / n),
           (min < 0) ? 0: min,
           max,
           n
    );
}
