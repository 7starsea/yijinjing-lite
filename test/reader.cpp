
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <sched.h>
#include <unistd.h>
#include "yijinjing/journal/JournalReader.h"
#include "yijinjing/journal/FrameHeader.h"
#include "yijinjing/journal/Frame.hpp"
#include "yijinjing/journal/Timer.h"

#include "stat.h"

using yijinjing::JournalReaderPtr;
using namespace yijinjing;

#define KUNGFU_JOURNAL_FOLDER "/tmp/yijinjing-lite/journal/"  /**< where we put journal files */



int main(){

        int cpu_id_ = 0;
        cpu_set_affinity(cpu_id_);


    std::vector<string> empty;
    JournalReaderPtr reader = yijinjing::JournalReader::create(KUNGFU_JOURNAL_FOLDER, "test", -1, "Client_R");
    
    bool flag = reader->seekTimeJournalByName("test", 0);
    std::cout<< "seekTimeJournalByName Return Flag:" << flag <<std::endl;
    getNanoTime();
    

    yijinjing::FramePtr frame;
    Calculator::print_header();
    int k = 0;
    while(k <= 10){
        Calculator cal;
        int count = 0;
        while (count < 8000)
        {
            frame = reader->getNextFrame();
            if (frame.get() != nullptr)
            {
                short msg_type = frame->getMsgType();            
                void* data = frame->getData();
                int64_t msg_time = frame->getNano();
                int len = frame->getDataLength();
                byte last_flag = frame->getLastFlag();

                if(msg_type == 11){
                    
                    int64_t cur_time = getNanoTime();
                    
                    cal.update(cur_time - msg_time);
                    ++count;
                }
            }
        }
        cal.print();
        
        ++k;
    }
    Calculator::print_footer();
}
