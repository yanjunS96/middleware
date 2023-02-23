#include "logger/mlog.h"
#include <unistd.h>

int main(int argc, char* argv[])
{
    //测试, 默认不调用写文件
    MLOG_INIT("./logger_config.toml", "test");

    MLOG_INFO("hello, world:{}", 12345);

    for (;;)
    {
        MLOG_DEBUG("hello, world");
        MLOG_INFO("hello, world");
        MLOG_WARN("hello, world");
        MLOG_ERROR("hello, world");
        MLOG_CRITICAL("hello, world");
//        decltype()

        usleep(100 * 1000);
    }

    return 0;
}
