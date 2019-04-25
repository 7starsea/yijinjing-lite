
#ifndef YIJINJING_TEST_STAT_H
#define YIJINJING_TEST_STAT_H


void cpu_set_affinity(int cpu_id);


class Calculator
{
public:
	static void print_header() ;
	static void print_footer() ;
private:
    int n;
    long min;
    long max;
    long sum;
    long square_sum;
public:
    Calculator();
    void update(long d);
   
    void print() const;
};


#endif