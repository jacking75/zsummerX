/*
 * zsummerX License
 * -----------
 * 
 * zsummerX is licensed under the terms of the MIT license reproduced below.
 * This means that zsummerX is free software and can be used for both academic
 * and commercial purposes at absolutely no cost.
 * 
 * 
 * ===============================================================================
 * 
 * Copyright (C) 2010-2015 YaweiZhang <yawei.zhang@foxmail.com>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * ===============================================================================
 * 
 * (end of COPYRIGHT)
 */

//! zsummer的测试服务模块(对应zsummer底层网络封装的上层设计测试服务) 可视为服务端架构中的 gateway服务/agent服务/前端服务, 特点是高并发高吞吐量
  //zsummer의 테스트 서비스 모듈 (zsummer 하단 네트워크 패키지의 상위 계층 설계 테스트 서비스에 해당)은 서버 측 아키텍처에서 게이트웨이 서비스 / 에이전트 서비스 / 프런트 엔드 서비스로 간주 될 수 있으며 높은 동시성 및 높은 처리량을 제공합니다.
//! Schedule头文件 该类负责监听端口, accept client, 并把accept到的socket分发给IOServer池去处理.
  //스케줄 클래스는 EventLoop로 클라이언트를 accept 처리를 하고 IOServer 풀에 수신된 소켓을 분배한다.


#ifndef ZSUMMER_SCHEDULE_H_
#define ZSUMMER_SCHEDULE_H_
#include "header.h"
class CProcess;
class CSchedule
{
public:
    CSchedule();
    
    void start();
    void stop();
    
    void run();

    void OnAccept(zsummer::network::NetErrorCode ec, TcpSocketPtr sockptr);

    void doConnect(unsigned int maxClient);

    //! IOServer
    std::vector<CProcess *> _process;
    int                        _iCurProcess = 0;


    zsummer::network::EventLoopPtr _summer;
    std::thread     _thread;
    zsummer::network::TcpAcceptPtr _accept;
    bool    _running = false;

};


#endif


