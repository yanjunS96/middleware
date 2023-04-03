#include <fstream>
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

#include "doip_control.h"

#define VER_CONFIG_FILE "/usr/motovis/log/module_version_log.txt"

using namespace motovis::doip;

typedef void (* SIGNALHANDLER)(int);

static bool isQuit = false;
//static std::condition_variable g_singleCon;
//static std::mutex g_singlemtx;
#if 0
void signal_hanlder(int signo)
{
#define BACKTRACE_DEEP  20 		// LEVEL

    int size = 0;
    char **strings = nullptr;
    void *array[BACKTRACE_DEEP];

    size = backtrace(array, BACKTRACE_DEEP);
    strings = (char **)backtrace_symbols(array, size);
    if(strings != nullptr)
    {
        for(int i = 0; i < size; i++)
        {
            MLOG_INFO("trace [{}] : {}", i, strings[i]);
        }
        free(strings);
    }

    if((signo == SIGTERM) || (signo == SIGINT) || (signo == SIGQUIT))
    {
        MLOG_WARN("signal_hanlder signo: {0:x}", signo);
        isQuit = true;
        {
            std::unique_lock<std::mutex> lck(g_singlemtx);
            g_singleCon.notify_one();
        }
    }
}
#endif
void signal_action(int signo, SIGNALHANDLER func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(signo != SIGALRM)
    {
        act.sa_flags |= SA_RESTART;
    }

    sigaction(signo, &act, &oact);
}

void sys_run_exit(int arg)
{
    MLOG_INFO("get exit single:{}", arg);
    exit(0);
}

int main(int argc, const char *argv[])
{
//    MLOG_INIT("./log/Config.yaml", "spi");
//    signal_action(SIGTERM, signal_hanlder);
//    signal_action(SIGHUP, signal_hanlder);
//    signal_action(SIGQUIT, signal_hanlder);
//    signal_action(SIGPIPE, signal_hanlder);
//    signal_action(SIGINT, signal_hanlder);

    signal_action(SIGTERM, sys_run_exit);
    signal_action(SIGHUP, sys_run_exit);
    signal_action(SIGQUIT, sys_run_exit);
    signal_action(SIGPIPE, sys_run_exit);
    signal_action(SIGINT, sys_run_exit);

#if 0 //关闭 spi1
    //1. 从文件解析需要的配置参数 并实例化功能类对象
    SpiInfo config_1 = SpiConfig((SpiMold::SPI1)).parseYaml();
    //实例化功能类对象
//        config_1.m_capacity = sizeof(SpiTxInfo); //暂时不支持传输字段修改
    SpiDevOne spiDevOne(config_1);
    spiDevOne.StartTask();
#endif
#if 0 //关闭 spi2
    SpiDevTwo spiDevTwo(SpiConfig((SpiMold::SPI2)).parseYaml());
    spiDevTwo.StartTask();
#endif
        //DoIp协议
#if 1 //暂不开启 DoIp 的功能
    DoIpInfoPtr doIpInfo(new DoIpInfo);
    DoipController doipcontrol(DoIpConfig(doIpInfo).parseYaml());
#endif
    while (!isQuit)
    {
        pause();
//            std::unique_lock<std::mutex> lck(g_singlemtx);
//            g_singleCon.wait(lck);
//            std::this_thread::sleep_for(std::chrono::seconds(TIMESYS_S));
    }
    return 0;
}
