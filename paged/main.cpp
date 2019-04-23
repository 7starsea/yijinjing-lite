
#include "PageEngine.h"


#define KUNGFU_FOLDER "/home/aimin/snap/kungfu/"                 /**< base folder of kungfu system */
#define KUNGFU_JOURNAL_FOLDER KUNGFU_FOLDER "journal/"  /**< where we put journal files */


#define TEMP_PAGE KUNGFU_JOURNAL_FOLDER "TEMP_PAGE"

#define COMM_FILE KUNGFU_JOURNAL_FOLDER "PAGE_ENGINE_COMM"



USING_YJJ_NAMESPACE;
int main(){

    yijinjing::PageEngine engine(COMM_FILE, TEMP_PAGE, KUNGFU_FOLDER );




    engine.set_freq(1);
    engine.start();

    return 0;
}