#include "logger/mlog.h"
#include "file_read/file_reader.hpp"
#include <unistd.h>

int main(int argc, char* argv[])
{
    //测试, 默认不调用写文件
    MLOG_INIT("./logger_config.toml", "test");

    MLOG_INFO("hello, world:{}", 12345);

    middleware::hoofs::FileReader fileReader("logger_config.toml");
    std::string read_val;

    for (;;)
    {
        fileReader.readLine(read_val);

        MLOG_DEBUG("hello, world,{}",read_val);
        MLOG_INFO("hello, world");
        MLOG_WARN("hello, world");
        MLOG_ERROR("hello, world{}",read_val);
        MLOG_CRITICAL("hello, world");
//        decltype()

        usleep(100 * 1000);
    }

    return 0;
}
