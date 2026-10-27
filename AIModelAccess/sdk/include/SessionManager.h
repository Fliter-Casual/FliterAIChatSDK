#pragma once 
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <memory>
#include "common.h"

namespace ai_chat_sdk
{

class SessionManager
{
public:
    // 创建会话，提供模型名称，返回会话ID
    std::string createSession(const std::string& modelName);
    // 通过会话ID获取会话信息
    std::shared_ptr<Session> getSession(const std::string& sessionId)const;
    // 给某个会话添加消息
    bool addMessage(const std::string& sessionId,const Message& message);
    // 获取某个会话的所有历史消息
    std::vector<Message> getHistroyMessage(const std::string& sessionId)const;
    // 更新会话的时间戳
    void updateSessionTimestamp(const std::string& sessionId);
    // 获取会话列表
    std::vector<std::string> getSessionLists()const;
    // 删除某个会话
    bool deleteSession(const std::string& sessionId);
    // 清空所有会话
    void clearAllSessions();
    // 获取所有会话总数
    size_t getSessionCount()const;

private:
    std::string generateSessionId();
    std::string generateMessageId(size_t messageCounter);

private:
    // key:会话ID,  value:会话信息
    std::unordered_map<std::string,std::shared_ptr<Session>> _sessions;
    mutable std::mutex _mutex; // mutable修饰的成员变量，在const修饰的成员函数中是可以被修改的
    std::atomic<int64_t> _sessionCounter = {0}; // 记录会话总数
};

} // namespace ai_chat_sdk