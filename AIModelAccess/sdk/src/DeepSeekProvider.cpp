#include "DeepSeekProvider.h"
#include "myLog.h"
#include <jsoncpp/json/json.h>
#include <httplib.h>

namespace ai_chat_sdk
{
    // DeepSeekProvider类
    bool DeepSeekProvider::initModel(const std::map<std::string, std::string> &modelConfig)
    {
        // 初始化API key
        auto it = modelConfig.find("api_key");
        if (it == modelConfig.end())
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
        if (it == modelConfig.end())
        {
            ERR("DeepSeekProvider initModel endpoint not fount");
            return false;
        }
        else
        {
            _endPoint = it->second;
        }

        _isAvailable = true;
        INFO("DeepSeekProvider initModel success, apiKey : %s, endpoint : %s", _apiKey.c_str(), _endPoint.c_str());
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
    std::string DeepSeekProvider::sendMessage(const std::vector<Message> &messages, const std::map<std::string, std::string> &request_param)
    {
        // 检查模型是否有效

        if (!_isAvailable)
        {
            ERR("DeepSeekProvider: model is not init!");
            return "";
        }
        // 获取采样温度和max_tokens
        double temperature = 0.7;
        int max_tokens = 2048;
        if (request_param.find("temperature") != request_param.end())
        {
            temperature = std::stof(request_param.at("temperature"));
        }
        if (request_param.find("max_tokens") != request_param.end())
        {
            max_tokens = std::stoi(request_param.at("max_tokens"));
        }
        // 构建历史消息
        Json::Value messages_array(Json::arrayValue);
        for (const auto &message : messages)
        {
            Json::Value msg;
            msg["role"] = message._role;
            msg["content"] = message._content;
            messages_array.append(msg);
        }
        // 构建请求体
        Json::Value request_body;
        request_body["model"] = "deepseek-chat";
        request_body["messages"] = messages_array; // 历史消息(塞到请求体中)
        request_body["temperature"] = temperature;
        request_body["max_tokens"] = max_tokens;

        // 序列化
        Json::StreamWriterBuilder writer; // 工具构建器
        std::string json_string = Json::writeString(writer, request_body);
        DBG("DeepSeekProvider: request_body: {}", json_string);

        // 使用cpp-httplib库构建HTTP客户端
        httplib::Client client(_endPoint);
        client.set_connection_timeout(30, 0); // 30秒超时
        client.set_read_timeout(60, 0);       // 60秒读取超时

        // 设置请求头
        httplib::Headers headers = {
            {"Authorization", "Bearer " + _apiKey},
            {"Content-Type", "application/json"}};

        // 发送POST请求,返回响应体
        auto response = client.Post("/v1/chat/completions", headers, json_string, "application/json");
        if (!response)
        {
            ERR("Failed to connect to DeepSeep API - check network and SSL");
            return "";
        }
        DBG("DeepSeek API response status: {}", response->status);
        DBG("DeepSeek API response body: {}", response->body);

        // 检查响应是否成功
        if (response->status != 200)
        {
            ERR("DeepSeek API returned non-200 status: {} - {}", response->status, response->body);
            return "";
        }

        // 解析响应体
        Json::Value response_json; // 输出参数
        Json::CharReaderBuilder reader_builder; // 也是一个工具构建器（建造者模式：如何构建一个及其复杂的对象）
        std::string parse_errors;
        std::istringstream response_stream(response->body); // 响应体流对象
        if (!Json::parseFromStream(reader_builder, response_stream, &response_json, &parse_errors))
        {
            ERR("Failed to parse DeepSeek API response: {}", parse_errors);
            return "";
        }

        // 解析大模型回复内容
        // 大模型回复包含在choicesjson数组中content
        if (response_json.isMember("choices") && response_json["choices"].isArray() && !response_json["choices"].empty())
        {
            auto &choice = response_json["choices"][0];
            if (choice.isMember("message") && choice["message"].isMember("content"))
            {
                std::string reply_content = choice["message"]["content"].asString();
                INFO("Received DeepSeek response: {}", reply_content);
                return reply_content;
            }
        }

        // 解析失败，返回错误信息
        ERR("Invalid response format from DeepSeek API");
        return "Invalid response format from DeepSeek API";
    }

    // 发送消息给模型 - 流式响应(增量返回)
    std::string DeepSeekProvider::sendMessageStream(const std::vector<Message> &messages,
                                                    const std::map<std::string, std::string> &request_param,
                                                    std::function<void(const std::string &, bool)> callback)
    {
        return "";
    }

} // namespace ai_chat_sdk