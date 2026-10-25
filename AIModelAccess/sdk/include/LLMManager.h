#pragma once
#include <map>
#include <memory>
#include "LLMProvider.h"


namespace ai_chat_sdk
{

class LLMManager
{
public:
    // 注册LLM提供商（把模型对象存入到管理器类的map-providers里面）
    bool registerProvider(const std::string& modelName,std::unique_ptr<LLMProvider> provider);
    // 初始化指定模型（模型名称用来找到模型，模型参数用来初始化模型）
    bool initModel(const std::string& modelName,const std::map<std::string,std::string>& modelParam);
    // 获取可用的模型
    std::vector<ModelInfo> getAvailableModels() const;
    // 检查模型是否可用
    bool isModelAvailable(const std::string& modelName)const;
    // 发送消息给指定模型
    std::string sendMessage(const std::string& modelName,const std::vector<Message>& messages,const std::map<std::string,std::string>& requestParam);
    // 发送流式消息
    std::string sendMessageStream(const std::string& modelName,const std::vector<Message>& messages,const std::map<std::string,std::string>& requestParam,const std::function<void(const std::string&, bool)>& callback);

private:
    // key: 模型名称, value: 模型提供器
    std::map<std::string,std::unique_ptr<LLMProvider>> _providers; // 这个就是注册的模型列表

    // key: 模型名称, value: 模型信息
    std::map<std::string,ModelInfo> _modelInfos;

};

}// namespace ai_chat_sdk