#include "../include/SessionManager.h"
#include <iomanip>
#include <sstream>
#include "../include/util/myLog.h"

namespace ai_chat_sdk
{

// 生成会话ID  会话id格式：时间戳 + 会话计数
std::string SessionManager::generateSessionId()
{
    // 会话计数原子自增
    _sessionCounter.fetch_add(1);
    std::time_t time = std::time(nullptr);
    // 生成会话id
    std::ostringstream oss;
    //  session_1234567890_000001
    oss << time << "_" << std::setw(6) << std::setfill('0') << (_sessionCounter);
    return oss.str();
}

// 生成消息ID   msg_1234567890_000001
std::string SessionManager::generateMessageId(size_t messageCounter)
{
    messageCounter++;
    std::time_t time = std::time(nullptr);
    std::ostringstream oos;
    oos<<"msg_"<<time<<"_"<<std::setw(6)<<std::setfill('0')<<messageCounter;
    return oos.str();
}

// 创建会话，提供模型名称，返回会话ID
std::string SessionManager::createSession(const std::string& modelName)
{
    std::lock_guard<std::mutex> lock(_mutex);

    // 生成会话id
    std::string sessionId = generateSessionId();

    // 创建会话，并设置会话id
    auto session = std::make_shared<Session>(modelName);
    session->_sessionId = sessionId;
    session->_createdAt = std::time(nullptr);
    session->_updatedAt = session->_createdAt;

    // 将会话入到会话列表
    _sessions[sessionId] = session;

    return sessionId;
}

// 通过会话ID获取会话信息
std::shared_ptr<Session> SessionManager::getSession(const std::string& sessionId)const
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(sessionId);
    if(it != _sessions.end()){
        _mutex.unlock();
        return it->second;
    }
}

// 给某个会话添加消息
bool SessionManager::addMessage(const std::string& sessionId,const Message& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    // 获取到sessionId对应的会话
    auto it = _sessions.find(sessionId);
    if(it == _sessions.end()){
        return false;
    }

    // 创建消息
    Message msg(message._role, message._content);
    msg._messageId = generateMessageId(it->second->_messages.size());
    msg._timestamp = std::time(nullptr);  // 设置消息时间戳
    INFO("message Info: content {}  timestamap {}", msg._content, msg._timestamp); 

    // 将消息添加到会话中
    it->second->_messages.push_back(msg);
    it->second->_updatedAt = std::time(nullptr);
    INFO("add message success, sessionId = {}, message.content = {}", sessionId, msg._content);

    return true;
}

// 获取某个会话的所有历史消息
std::vector<Message> SessionManager::getHistroyMessage(const std::string& sessionId)const
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(sessionId);
    if(it == _sessions.end()){
        return {};
    }
    return it->second->_messages;
}

// 更新会话的时间戳
void SessionManager::updateSessionTimestamp(const std::string& sessionId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(sessionId);
    if(it == _sessions.end()){
        return;
    }
    it->second->_updatedAt = std::time(nullptr);
}

// 获取会话列表
std::vector<std::string> SessionManager::getSessionLists()const
{
    
}

// 删除某个会话
bool SessionManager::deleteSession(const std::string& sessionId)
{

}

// 清空所有会话
void SessionManager::clearAllSessions()
{

}

// 获取所有会话总数
size_t SessionManager::getSessionCount() const
{

}

}