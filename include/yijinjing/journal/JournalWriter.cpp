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
 * JournalWriter
 * @Author cjiang (changhao.jiang@taurus.ai)
 * @since   March, 2017
 * enable user to write in journal.
 * one journal writer can only write one journal,
 * and meanwhile this journal cannot be linked by other writer
 */

#include "JournalWriter.h"
#include "Journal.h"
#include "PageProvider.h"
#include "Timer.h"
#include "yijinjing/utils/constants.h"
///#include <mutex> // used by JournalSafeWriter

USING_YJJ_NAMESPACE

const string JournalWriter::PREFIX = "writer";

void JournalWriter::init(const string& dir, const string& jname)
{
    addJournal(dir, jname);
    journal = journals[0];
    seekEnd();
}

short JournalWriter::getPageNum() const
{
    return journal->getCurPageNum();
}

long JournalWriter::writeStr(const string& str)
{
    return write_frame(str.c_str(), str.length() + 1, YJJ_INTERNAL_MSG_TYPE_STRING, 1);
}

long JournalWriter::write_frame(const void* data, FH_TYPE_LENGTH length,  FH_TYPE_MSG_TP msgType,
                                      FH_TYPE_LASTFG lastFlag)
{
    long nano = getNanoTime();
    
    void* buffer = journal->locateFrame();
    Frame frame(buffer);
    frame.setMsgType(msgType);
    frame.setLastFlag(lastFlag);
    frame.setData(data, length);
    frame.setNano(nano);
    frame.setStatusWritten();
    journal->passFrame();
    return nano;
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname, const string& writerName)
{
    PageProviderPtr provider = PageProviderPtr(new PageProvider(writerName, true));

    return JournalWriter::create(dir, jname, provider);
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname, PageProviderPtr provider)
{
    JournalWriterPtr jwp = JournalWriterPtr(new JournalWriter(provider));
    jwp->init(dir, jname);
    return jwp;
}

JournalWriterPtr JournalWriter::create(const string& dir, const string& jname)
{
    return JournalWriter::create(dir, jname, getDefaultName(JournalWriter::PREFIX));
}

void JournalWriter::seekEnd()
{
    journal->seekTime(TIME_TO_LAST);
    journal->locateFrame();
}

//std::mutex safe_writer_mtx;
//
//long JournalSafeWriter::write_frame(const void* data, FH_TYPE_LENGTH length,  FH_TYPE_MSG_TP msgType,
//                                         FH_TYPE_LASTFG lastFlag)
//{
//    std::lock_guard<std::mutex> lck (safe_writer_mtx);
//    return this->JournalWriter::write_frame(data, length, msgType, lastFlag);
//}
//
//JournalWriterPtr JournalSafeWriter::create(const string& dir, const string& jname, const string& writerName)
//{
//    PageProviderPtr provider = PageProviderPtr(new ClientPageProvider(writerName, true));
//    JournalWriterPtr jwp = JournalWriterPtr(new JournalSafeWriter(provider));
//    jwp->init(dir, jname);
//    return jwp;
//}
