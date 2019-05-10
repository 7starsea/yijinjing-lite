
#include <iostream>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "yijinjing/journal/JournalWriter.h"
#include "yijinjing/journal/FrameHeader.h"
#include "yijinjing/journal/Timer.h"
#include "yijinjing/journal/PageProvider.h"


#include "stat.h"


using yijinjing::JournalWriterPtr;
using namespace yijinjing;


#define KUNGFU_JOURNAL_FOLDER "/tmp/yijinjing-lite/journal/"  /**< where we put journal files */



int main(int argc, char *argv[]){
    
        int cpu_id_ = 1;
        cpu_set_affinity(cpu_id_);

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
    while(k <= 15){
        
        ///data += 2;
        Calculator cal;
        
        int count = 4000;
        for(int j = 0; j < count; ++j){
            int64_t nano = writer_->write_frame(data, len, 11, 0);
            
            for(int i = 0; i < 1000; ++i){
                writer_->write_frame(data, len, 11, 0);
            }
            writer_->write_frame(data, len, 11, 1);

            int64_t cur_time = getNanoTime() - nano;
            cal.update(cur_time);
            usleep(10);
        }
        cal.print();
        ++k;
        usleep(yijinjing::MICROSECONDS_PER_SECOND);
    }
    
    Calculator::print_footer();

}
