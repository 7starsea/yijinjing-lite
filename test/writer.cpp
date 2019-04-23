
#include <iostream>
#include <string.h>
#include "journal/JournalWriter.h"
#include "journal/FrameHeader.h"
#include "journal/Timer.h"
#include "journal/PageProvider.h"

using yijinjing::JournalWriterPtr;

#define KUNGFU_FOLDER "/home/aimin/snap/kungfu/"                 /**< base folder of kungfu system */
#define KUNGFU_JOURNAL_FOLDER KUNGFU_FOLDER "journal/"  /**< where we put journal files */


#define BL_BASE_FOLDER KUNGFU_JOURNAL_FOLDER "strategy/"

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


    JournalWriterPtr writer_ = yijinjing::JournalWriter::create(BL_BASE_FOLDER, "test", "Client");


    int len = 10;
    if(argc > 1){
    	len = atoi(argv[1]);
    }
    if(len <= 0) len = 10;
    std::cout<<"DataLength:"<<len<<std::endl;

    char * data = new char[len];
    data[len-1]=0;
    strncpy(data, "This is a test", len - 1);
    while(true){
        ///data += 2;
        writer_->write_frame(data, len, 11, 0);

        usleep(yijinjing::MICROSECONDS_PER_SECOND);
    }

}