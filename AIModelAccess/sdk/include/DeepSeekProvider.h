#include "LLMProvider.h"


namespace ai_chat_sdk
{
class DeepSeekProvider : public LLMProvider
{
public:
    // 初始化模型
    virtual bool initModel(const std::map<std::string, std::string>& modelConfig) override;
    // 检测模型是否有效
    virtual bool isAvailable() const override;
    // 获取模型名称
    virtual std::string getModelName() const override;
    // 获取模型描述信息
    virtual std::string getModelDesc() const override;
    // 发送消息给模型 - 全量返回
    virtual std::string sendMessage(const std::vector<Message>& messages, const std::map<std::string, std::string>& request_param);
    // 发送消息给模型 - 流式响应(增量返回)
    virtual std::string sendMessageStream(const std::vector<Message>& messages, 
                                        const std::map<std::string, std::string>& request_param, 
                                        std::function<void(const std::string&, bool)> callback); // callback:对模型返回的增量数据如何处理，第一个参数为增量数据，第二个参数为是否为最后一个增量数据
};

} // namespace ai_chat_sdk