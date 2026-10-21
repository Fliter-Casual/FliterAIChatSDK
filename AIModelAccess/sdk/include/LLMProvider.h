#include <string>
#include <functional>
#include <map>
#include <vector>
#include "common.h"

namespace ai_chat_sdk
{
    // LLMProvider 类
class LLMProvider
{
public:
    // 初始化模型
    virtual bool initModel(const std::map<std::string, std::string>& modelConfig) = 0;
    // 检测模型是否有效
    virtual bool isAvailable() = 0;
    // 获取模型名称
    virtual std::string getModelName()const = 0;
    // 获取模型描述信息
    virtual std::string getModelDesc()const = 0;
    // 发送消息给模型 - 全量返回
    virtual std::string sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& request_param) = 0;
    // 发送消息给模型 - 流式响应(增量返回)
    virtual std::string sendMessageStream(const std::vector<Message>& messages, 
                                        const std::map<std::string, std::string>& request_param, 
                                        std::function<void(const std::string&, bool)> callback) = 0; // callback:对模型返回的增量数据如何处理，第一个参数为增量数据，第二个参数为是否为最后一个增量数据
protected:
    bool _isAvailable = false;     // 标记模型是否有效
    std::string _apiKey;          // 保存模型的API Key 
    std::string _endPoint;         // 保存模型的base url 
};


} // namespace ai_chat_sdk