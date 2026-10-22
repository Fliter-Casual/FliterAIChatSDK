#pragma once
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <mutex>

namespace fliter
{

class Logger // 封装成单例模式，构造方法变成私有的,把拷贝构造和赋值运算符重载禁止掉
{
public:
    static void initLogger(const std::string& loggerName,const std::string& loggerFile,spdlog::level::level_enum = spdlog::level::info);
    static std::shared_ptr<spdlog::logger> getLogger();
private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
private:
    // 创建一个静态的类对象，或者一个静态的方法来获取这个实例
    static std::shared_ptr<spdlog::logger> _logger;
    static std::mutex _mutex;
};

#define TRACE(format, ...) fliter::Logger::getLogger()->trace(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)
#define DBG(format, ...) fliter::Logger::getLogger()->debug(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO(format, ...) fliter::Logger::getLogger()->info(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN(format, ...) fliter::Logger::getLogger()->warn(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERR(format, ...) fliter::Logger::getLogger()->error(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)
#define CRIT(format, ...) fliter::Logger::getLogger()->critical(std::string("[{:>10s}:{:<4d}] ")+format, __FILE__, __LINE__, ##__VA_ARGS__)

} // namespace fliter