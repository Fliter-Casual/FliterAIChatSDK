#include <gtest/gtest.h>
#include "../sdk/include/DeepSeekProvider.h"
#include "../sdk/include/util/myLog.h"

TEST(DeepSeekProviderTest, sendMessage) {
    auto provider = std::make_shared<ai_chat_sdk::DeepSeekProvider>();
    ASSERT_TRUE(provider != nullptr); //gtest中的断言宏

    std::map<std::string, std::string> modelParams = {
        {"api_key", std::getenv("DEEPSEEK_API_KEY")},
        {"endpoint", "https://api.deepseek.com"},
    };
    provider->initModel(modelParams);
    ASSERT_TRUE(provider->isAvailable());
    std::map<std::string, std::string> requestParams = {
        {"temperature", "0.7"},
        {"max_tokens", "2048"},
    };
    std::vector<ai_chat_sdk::Message> messages;
    messages.push_back(ai_chat_sdk::Message("user", "你好"));
    
    //std::string response = provider->sendMessage(messages,requestParams);

    // 回调函数给调用sendMessageStream的该层用户返回增量文本 (如何去显示自己决定)
    auto writeChunk = [&](const std::string& chunk,bool last)
    {
        // 每触发一次回调就打印一次
        INFO("chunk : {}",chunk);
        if(last)
        {
            INFO("[DONE]");
        }
    };
    std::string fullData = provider->sendMessageStream(messages,requestParams,writeChunk);
    ASSERT_TRUE(!fullData.empty()); 
    // 最后打印返回的完整数据
    INFO("response : {}", fullData);
}

int main(int argc, char **argv) {
    // 初始化 spdlog 日志库
    fliter::Logger::initLogger("testLLM","stdout",spdlog::level::debug);

    // 初始化 google test
    ::testing::InitGoogleTest(&argc, argv);

    // 执行所有的测试用例
    return RUN_ALL_TESTS();
}