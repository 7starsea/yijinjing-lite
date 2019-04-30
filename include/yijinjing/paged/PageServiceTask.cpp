/*****************************************************************************
 * Copyright [2017] [taurus.ai]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

/**
 * Page Service Tasks.
 * @Author cjiang (changhao.jiang@taurus.ai)
 * @since   March, 2017
 * Page engine needs several tasks to be done in schedule.
 * here we define tasks which can be implemented in page engine
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>

#include "PageServiceTask.h"
#include "PageEngine.h"
#include "yijinjing/journal/PageUtil.h"
//#include "journal/Timer.h"
//#ifdef ENABLE_ACTIVATION_CODE
//#include "ActivationCode.hpp"
//#endif

USING_YJJ_NAMESPACE


PstPidCheck::PstPidCheck(PageEngine *pe): engine(pe) {}

void PstPidCheck::go()
{
    vector<string> clientsToRemove;
    {
        for (auto const &item: engine->pidClient)
        {
            struct stat sts;
            std::stringstream ss;
            ss << "/proc/" << item.first;
            if (stat(ss.str().c_str(), &sts) == -1 && errno == ENOENT)
            {
                for (auto const &name: item.second)
                {
                    clientsToRemove.push_back(name);
                }
            }
        }
    }
    for (auto const &name: clientsToRemove)
    {
        engine->exit_client(name, 0, false);
    }
}


PstTempPage::PstTempPage(PageEngine *pe, const string & temp_page_path): engine(pe), PageFullPath(temp_page_path) {
}

void PstTempPage::go()
{
    auto &fileAddrs = engine->fileAddrs;
    if (fileAddrs.find(PageFullPath) == fileAddrs.end())
    {
        engine->get_logger()->info("NEW TEMP PAGE: {}",  PageFullPath);
        void *buffer = PageUtil::LoadPageBuffer(PageFullPath, JOURNAL_PAGE_SIZE, true, true);
        if (buffer != nullptr)
        {
            fileAddrs[PageFullPath] = buffer;
        }
    }
}
