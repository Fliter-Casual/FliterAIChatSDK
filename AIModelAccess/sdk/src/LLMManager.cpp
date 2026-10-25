#include "../include/LLMManager.h"
#include "../include/util/myLog.h"
#include "../include/common.h"

namespace ai_chat_sdk
{
    // 注册LLM提供者
    bool LLMManager::registerProvider(const std::string& modelName, std::unique_ptr<LLMProvider> provider)
    {
        // 参数检测
        if(!provider){
            ERR("cannot register nullptr provider, modelName = {}", modelName);
            return false;
        }

        // 资源转移
        _providers[modelName] = std::move(provider);

        // 添加模型信息
        _modelInfos[modelName] = ModelInfo(modelName); // 只先初始化了名称

        // 模型初始化成功
        INFO("register provider success, modelName = {}", modelName);
        return true;
    }

    // 初始化指定模型
    bool LLMManager::initModel(const std::string& modelName, const std::map<std::string, std::string>& modelParam)
    {
        // 检测模型是否注册
        auto it = _providers.find(modelName);
        if(it == _providers.end())
        {
            ERR("model provider not found, modelName = {}", modelName);
            return false;
        }

        // 模型已注册，可以初始化该模型 (除了name,还有endpoint和apikey)
        bool isSuccess = it->second->initModel(modelParam);
        if(!isSuccess)
        {
            INFO("Model {} init Failed!!!", modelName);
            return false;
        }
        else // 初始化模型，添加模型的各种配置信息
        {
            INFO("Model {} init success!!!", modelName);
            _modelInfos[modelName]._desc = it->second->getModelDesc();
            _modelInfos[modelName]._isAvailable = true;
        }
        
        return isSuccess;
    }

    // 获取可用模型列表
    std::vector<ModelInfo> LLMManager::getAvailableModels()const
    {
        //从注册的模型列表_providers中筛选出所有可用的模型
        std::vector<ModelInfo> models;
        for(const auto& pair : _modelInfos)
        {
            if(pair.second._isAvailable)
            {
                models.push_back(pair.second);
            }
        }
        return models;
    }

    // 检测模型是否可用
    bool LLMManager::isModelAvailable(const std::string& modelName)const
    {
        auto it = _modelInfos.find(modelName);
        return it != _modelInfos.end() && it->second._isAvailable;
    }

    //发送消息到指定模型
    std::string LLMManager::sendMessageStream(const std::string& modelName,
                                        const std::vector<Message>& messages,
                                        const std::map<std::string, std::string>&requestParams,
                                        const std::function<void(const std::string&, bool)>& callback)
    {
        // 检测模型是否注册
        auto it = _providers.find(modelName); // 通过名字找到这个模型本身，然后调用其内部的sendMessage发送请求
        if(it == _providers.end())
        {
            ERR("model provider is not found, modelName{}", modelName);
            return "";
        }
        
        // 检测模型是否可用
        if(!isModelAvailable(modelName))
        {
            ERR("model is not available, modelName{}", modelName);
            return "";
        }

        // 模型已经注册并初始化成功
        return it->second->sendMessageStream(messages,requestParams,callback);
    }



}// namespace ai_chat_sdk