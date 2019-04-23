
#include <iostream>
#include <fstream>
#include <string.h>
#include <sched.h>
#include "journal/JournalReader.h"
#include "journal/FrameHeader.h"
#include "journal/Frame.hpp"
#include "journal/Timer.h"

using yijinjing::JournalReaderPtr;
using namespace yijinjing;

#define KUNGFU_FOLDER "/home/aimin/snap/kungfu/"                 /**< base folder of kungfu system */
#define KUNGFU_JOURNAL_FOLDER KUNGFU_FOLDER "journal/"  /**< where we put journal files */


#define BL_BASE_FOLDER KUNGFU_JOURNAL_FOLDER "strategy/"

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
    JournalReaderPtr reader = yijinjing::JournalReader::create(BL_BASE_FOLDER, "test", -1, "Client_R");
    
    getNanoTime();
    
//    reader->addJournal(BL_BASE_FOLDER, "test");

    yijinjing::FramePtr frame;

    std::vector<int> delay; int count = 0;
    while (count <= 40)
    {
        frame = reader->getNextFrame();
        if (frame.get() != nullptr)
        {
            short msg_type = frame->getMsgType();
            std::string name = reader->getFrameName();
            void* data = frame->getData();
            long long msg_time = frame->getNano();
            int len = frame->getDataLength();

            if(msg_type == 11){
                long long cur_time = getNanoTime();
                delay.push_back( cur_time - msg_time );
                count += 1;
    //            std::cout<<data<<" "<<len<<std::endl;
            }
        }
    }


    std::ofstream ofs("delay.txt", std::ostream::app);
    for(auto it = delay.begin(); it != delay.end(); ++it){
        ofs<<*it<<",";
    }
    std::cout<<std::endl;
    ofs.close();
}