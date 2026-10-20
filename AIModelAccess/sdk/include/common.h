#pragma once
#include <string>
#include <ctime>
#include <vector>

namespace ai_chat_sdk
{

    // 消息结构
    struct Message
    {
        std::string _emssageId; // 消息ID
        std::string _role;      // 角色,如user、assistant等
        std::string _content;   // 消息内容
        std::string _timestamp; // 消息发送的时间戳

        Message(const std::string &r, const std::string &c)
            : _role(r), _content(c)
        {
        }
    };

    // 模型的公共配置信息config
    struct Config
    {
        virtual ~Config() = default; // 确保多态销毁时安全
        std::string _modelName;
        double _temperature = 0.7; // 温度参数，用于控制生成文本的随机性
        int _maxTokens = 2048;
    };

    // 通过api的方式接入云端模型
    // Ollama本地接⼊⼤模型，不需要api key，因此将api key单独拎出来
    struct ApiConfig : public Config
    {
        std::string api_key; // 接⼊模型时认证信息
    };

    // 通过Ollama接入本地模型(不需要apikey)

    // LLM模型信息
    struct ModelInfo
    {
        std::string _name;     // 模型名称
        std::string _desc;     // 模型描述
        std::string _provider; // 模型提供者
        std::string _endpoint; // 模型base url
        bool _isAvailable;     // 模型是否被初始化

        ModelInfo(const std::string &name, const std::string &desc = "", const std::string provider = "", const std::string endpoint = "")
            : _name(name), _desc(desc), _provider(provider), _endpoint(endpoint), _isAvailable(false)
        {}
    };

    // 会话信息
    struct Session
    {
        std::string _sessionId;         // 会话ID
        std::string _modeName;         // 模型名称
        std::vector<Message> _messages;  // 消息列表
        std::time_t _createdAt;         // 会话创建时间戳
        std::time_t _updatedAt;         // 会话最后更新时间戳
        Session(const std::string &modelName = "")
            : _modeName(modelName)
        {}
    };

} // namespace ai_chat_sdk
