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
 * Page Provider.
 * @Author cjiang (changhao.jiang@taurus.ai)
 * @since   March, 2017
 * implements IPageProvider, diverge to different usage
 */

#include "PageProvider.h"
#include "PageCommStruct.h"
#include "PageSocketStruct.h"
#include "PageUtil.h"
#include "Page.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>

USING_YJJ_NAMESPACE

typedef boost::array<char, SOCKET_MESSAGE_MAX_LENGTH> SocketMArray;


/** get socket response via paged_socket */
void getSocketRsp(SocketMArray &input, SocketMArray &output)
{
    using namespace boost::asio;
    io_service io_service;
    local::stream_protocol::socket socket(io_service);
    socket.connect(local::stream_protocol::endpoint(PAGED_SOCKET_FILE));
    boost::system::error_code error;
    write(socket, buffer(input), error);
    socket.read_some(buffer(output), error);
}

/** send req via socket and get response in data */
void getSocketRspOnReq(PagedSocketRequest& req, SocketMArray& data, const string& name)
{
    memcpy(req.name, name.c_str(), name.length() + 1);
    SocketMArray reqBuf;
    memcpy(&reqBuf[0], &req, sizeof(req));
    getSocketRsp(reqBuf, data);
}

PageProvider::PageProvider(const string& clientName, bool isWriting, bool reviseAllowed):
is_writer(isWriting),revise_allowed(is_writer || reviseAllowed)
, client_name(clientName), comm_buffer(nullptr), hash_code(0)
{
    register_client();
}

void PageProvider::register_client()
{
    PagedSocketRequest req = {};
    req.type = is_writer ? PAGED_SOCKET_WRITER_REGISTER : PAGED_SOCKET_READER_REGISTER;
    req.pid = getpid();
    SocketMArray rspArray;
    getSocketRspOnReq(req, rspArray, client_name);
    PagedSocketRspClient* rsp = (PagedSocketRspClient*)(&rspArray[0]);
    hash_code = rsp->hash_code;
    if (rsp->type == req.type && rsp->success)
    {
        comm_buffer = PageUtil::LoadPageBuffer(string(rsp->comm_file), rsp->file_size, true, false /*server lock this already*/);
    }
    else
    {
        throw std::runtime_error("cannot register client: " + client_name);
    }
}

void PageProvider::exit_client()
{// send message to say good bye
    PagedSocketRequest req = {};
    req.type = PAGED_SOCKET_CLIENT_EXIT;
    req.hash_code = hash_code;
    SocketMArray rspArray;
    getSocketRspOnReq(req, rspArray, client_name);
}

int PageProvider::register_journal(const string& dir, const string& jname)
{
    PagedSocketRequest req = {};
    req.type = PAGED_SOCKET_JOURNAL_REGISTER;
    SocketMArray rspArray;
    getSocketRspOnReq(req, rspArray, client_name);
    PagedSocketRspJournal* rsp = (PagedSocketRspJournal*)(&rspArray[0]);
    int comm_idx = -1;
    if (rsp->type == req.type && rsp->success)
        comm_idx = rsp->comm_idx;
    else
        throw std::runtime_error("cannot register journal: " + client_name);

    PageCommMsg* serverMsg = GET_COMM_MSG(comm_buffer, comm_idx);
    if (serverMsg->status == PAGED_COMM_OCCUPIED)
    {
        memcpy(serverMsg->folder, dir.c_str(), dir.length() + 1);
        memcpy(serverMsg->name, jname.c_str(), jname.length() + 1);
        serverMsg->is_writer = is_writer;
        serverMsg->status = PAGED_COMM_HOLDING;
    }
    else
        throw std::runtime_error("server buffer is not allocated: " + client_name);

    return comm_idx;
}

PagePtr PageProvider::getPage(const string &dir, const string &jname, int serviceIdx, short pageNum)
{
    PageCommMsg* serverMsg = GET_COMM_MSG(comm_buffer, serviceIdx);
    serverMsg->page_num = pageNum;
    serverMsg->status = PAGED_COMM_REQUESTING;
    while (serverMsg->status == PAGED_COMM_REQUESTING) {}

    if (serverMsg->status != PAGED_COMM_ALLOCATED)
    {
        if (serverMsg->status == PAGED_COMM_MORE_THAN_ONE_WRITE)
            throw std::runtime_error("more than one writer is writing " + dir + " " + jname);
        else
            return PagePtr();
    }
    return Page::load(dir, jname, pageNum, revise_allowed, true);
}

void PageProvider::releasePage(void* buffer, int size, int serviceIdx)
{
    PageUtil::ReleasePageBuffer(buffer, size, true);
}


