#include "DeepSeekProvider.h"
#include "myLog.h"

namespace ai_chat_sdk
{
    // DeepSeekProvider类
    bool DeepSeekProvider::initModel(const std::map<std::string,std::string>& modelConfig)
    {
        // 初始化API key
        auto it = modelConfig.find("api_key");
        if(it == modelConfig.end())
        {
            ERR("DeepSeekProvider initModel api_key not fount");
            return false;
        }
        else
        {
            _apiKey = it->second;
        }

        // 初始化Base URL(endpoint)
        it = modelConfig.find("endpoint");
        if(it == modelConfig.end())
        {
            ERR("DeepSeekProvider initModel endpoint not fount");
            return false;
        }
        else
        {
            _endPoint = it->second;
        }

        _isAvailable = true;
        INFO("DeepSeekProvider initModel success, apiKey : %s, endpoint : %s", _apiKey.c_str() , _endPoint.c_str());
        return true;
    }

    // 检测模型是否可用
    bool DeepSeekProvider::isAvailable() const
    {
        return _isAvailable;
    }

    // 获取模型名称
    std::string DeepSeekProvider::getModelName() const
    {
        return "deepseek-chat";
    }

    // 获取模型的描述信息
    std::string DeepSeekProvider::getModelDesc() const
    {
        return "⼀款实⽤性强、中⽂优化的通⽤对话助⼿，适合⽇常问答与创作";
    }

    // 发送消息给模型 - 全量返回
    void DeepSeekProvider::sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& request_param)
    {

    }
    // 发送消息给模型 - 流式响应(增量返回)
    void DeepSeekProvider::sendMessageStream(const std::vector<Message>& messages, 
                                             const std::map<std::string, std::string>& request_param, 
                                             std::function<void(const std::string&, bool)> callback)
    {

    }
    
} // namespace ai_chat_sdk