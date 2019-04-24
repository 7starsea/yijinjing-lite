
#include <iostream>
#include <string.h>
#include <math.h>
#include <sched.h>
#include "journal/JournalWriter.h"
#include "journal/FrameHeader.h"
#include "journal/Timer.h"
#include "journal/PageProvider.h"

using yijinjing::JournalWriterPtr;
using namespace yijinjing;


#define KUNGFU_JOURNAL_FOLDER "/tmp/yijinjing-lite/journal/"  /**< where we put journal files */


class Calculator
{
	public:
	static void print_header() {
		std::cout << "+--------------+--------------+--------------+--------------+-------+" << std::endl;
		std::cout << " mean(ns)     | std(ns)      | min(ns)      | max(ns)      | valid |" << std::endl;
		std::cout << "--------------+--------------+--------------+--------------+-------+" << std::endl;		
		}
	static void print_footer() {
        std::cout << "+--------------+--------------+--------------+--------------+-------+" << std::endl;		
		}
private:
    int n;
    long min;
    long max;
    long sum;
    long square_sum;
public:
    Calculator(): n(0), min(1000000000), max(0), sum(0), square_sum(0) {}
    void update(long d)
    {
        ++n;
        sum += d;
        square_sum += (long)d*d;
        ///min = (min < 0) ? d : ((d < min) ? d : min);
        if(d<min) min = d;
        if(d>max) max = d;
//        min = (d < min) ? d : min;
//        max = (d > max) ? d : max;
    };

    void print() const
    {
        printf("%14ld|%14.5e|%14ld|%14ld|%7d|\n",
               (n == 0)? 0: sum / n,
               (n == 0)? 0: sqrt((square_sum - (double)sum * sum / n) / n),
               (min < 0) ? 0: min,
               max,
               n
        );
    }
};



int main(int argc, char *argv[]){
    
        int cpu_id_ = 1;
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(cpu_id_, &mask);
        if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
            std::cout<<"set cpu affinity failed, cpu_id " << cpu_id_ << std::endl;
        }
        else{
            std::cout<<"set cpu affinity success, cpu_id " << cpu_id_ << std::endl;
        }

    getNanoTime();
    JournalWriterPtr writer_ = yijinjing::JournalWriter::create(KUNGFU_JOURNAL_FOLDER, "test", "Client");


    int len = 10;
    if(argc > 1){
    	len = atoi(argv[1]);
    }
    if(len <= 0) len = 10;
    std::cout<<"DataLength:"<<len<<std::endl;

    char * data = new char[len];
    data[len-1]=0;
    strncpy(data, "This is a test", len - 1);

	Calculator::print_header();
	int k = 0;
    while(k <= 10){
        
        ///data += 2;
        Calculator cal;
        
        int count = 2000;
        for(int i = 0; i < count; ++i){
            long long nano = writer_->write_frame(data, len, 11, 0);
            long long cur_time = getNanoTime() - nano;
            
            cal.update(cur_time);
            
            usleep(1);
        }

        cal.print();
        usleep(yijinjing::MICROSECONDS_PER_SECOND);
    }
    
    Calculator::print_footer();

}
