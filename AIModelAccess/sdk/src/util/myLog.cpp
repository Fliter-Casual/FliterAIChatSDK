#include "../../include/util/myLog.h"


namespace fliter
{
    static std::shared_ptr<spdlog::logger> _logger = nullptr;
    static std::mutex _mutex;

    Logger::Logger()
    {}

    void initLogger(const std::string &loggerName, const std::string &loggerFile, spdlog::level::level_enum logLevel)
    {
        if (nullptr == _logger)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (nullptr == _logger)
            {
                // 设置全局自动刷新级别
                spdlog::flush_on(logLevel);
                // 异步输出日志，即将日志信息存放到队列中,有后台线程负责写入(消费到具体的文件当中)
                // 参数：队列大小、后台线程数量
                spdlog::init_thread_pool(32768, 1);
                if ("stdout" == loggerFile)
                {
                    // 创建一个带颜色的输出到控制台的日志器
                    _logger = spdlog::stdout_color_mt(loggerName);
                }
                else
                {
                    // 创建⼀个输出到指定⽂件的异步⽇志器
                    _logger = spdlog::basic_logger_mt<spdlog::async_logger>(loggerName, loggerFile);
                }
            }

            // 格式设置
            // %H:%M:%S 时分秒
            // %n ⽇志器的名称
            // %l: ⽇志级别的名字info、error，%-7l：左对⻬占7个字符
            // %v: ⽇志消息正⽂
            _logger->set_pattern("%H:%M:%S [%n][%-7l]%v");
            _logger->set_level(logLevel);
        }
    }

    std::shared_ptr<spdlog::logger> Logger::getLogger()
    {
        return _logger;
    }

}// namespace fliter
