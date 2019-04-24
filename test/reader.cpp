
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <sched.h>
#include "journal/JournalReader.h"
#include "journal/FrameHeader.h"
#include "journal/Frame.hpp"
#include "journal/Timer.h"

using yijinjing::JournalReaderPtr;
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


int main(){
        int cpu_id_ = 0;
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(cpu_id_, &mask);
        if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
            std::cout<<"set cpu affinity failed, cpu_id " << cpu_id_ << std::endl;
        }
        else{
            std::cout<<"set cpu affinity success, cpu_id " << cpu_id_ << std::endl;
        }

    std::vector<string> empty;
    JournalReaderPtr reader = yijinjing::JournalReader::create(KUNGFU_JOURNAL_FOLDER, "test", -1, "Client_R");
    
    getNanoTime();
    
//    reader->addJournal(KUNGFU_JOURNAL_FOLDER, "test");

    yijinjing::FramePtr frame;
	
	Calculator cal;
	int count = 0;
    while (count <= 4000)
    {
        frame = reader->getNextFrame();
        if (frame.get() != nullptr)
        {
            short msg_type = frame->getMsgType();            
            void* data = frame->getData();
            long long msg_time = frame->getNano();
            int len = frame->getDataLength();

            if(msg_type == 11){
                long long cur_time = getNanoTime();
                
                cal.update(cur_time - msg_time);
                ++count;
            }
        }
    }

    
    Calculator::print_header();
    cal.print();
    Calculator::print_footer();
}
