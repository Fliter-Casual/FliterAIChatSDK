#include "../include/DeepSeekProvider.h"
#include "../include/util/myLog.h"
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
        INFO("DeepSeekProvider initModel success, apiKey : {}, endpoint : {}", _apiKey, _endPoint);
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
        return "一款好用、对中文支持好的通用对话助手，日常问答和写作都能胜任。";
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
                                                    std::function<void(const std::string &, bool)> callback) // callback这个参数用来处理解析出来的数据
    {
        INFO("DeepSeekProvider sendMessageStream");
        if(!isAvailable())
        {
            ERR("DeepSeekProvider is not available");
            return "";
        }

        // 构造请求参数
        double temperature = 0.7;
        int max_tokens = 2048;
        if(request_param.find("temperature") != request_param.end())
        { 
            temperature = std::stof(request_param.at("temperature")); 
        }
        if(request_param.find("max_tokens") != request_param.end())
        { 
            max_tokens = std::stoi(request_param.at("max_tokens"));
        }

        // 构建历史消息(包含当前对话)
        Json::Value messages_array(Json::arrayValue);
        for(const auto& message : messages)
        {
            Json::Value msg;
            msg["role"] = message._role;
            msg["content"] = message._content;
            messages_array.append(msg);
        }

        // 构建请求体
        Json::Value request_body;
        request_body["model"] = "deepseek-chat";
        request_body["messages"] = messages_array; // 消息
        request_body["temperature"] = temperature;
        request_body["max_tokens"] = max_tokens;
        request_body["stream"] = true; // 开启流式响应

        // 序列化
        Json::StreamWriterBuilder writer;
        std::string json_string = Json::writeString(writer, request_body);
        DBG("DeepSeekProvider: Send stream request to deepseek Server,request_body: {}", json_string);

        //创建HTTP Client
        httplib::Client client(_endPoint);
        client.set_connection_timeout(30,0); // 30秒超时
        client.set_read_timeout(300, 0);//流式响应需要更⻓的时间

        //设置请求头
        httplib::Headers headers = {
            {"Authorization", "Bearer " + _apiKey},
            {"Content-Type", "application/json"},
            {"Accept", "text/event-stream"}
        };
        // 设置完请求头过后还不能发送POST请求
        // 要先设置响应处理器和内容接收处理器

        // 流式处理变量
        std::string buffer;       //接收流式响应的数据块
        bool gotError = false;    //响应是否成功
        std::string errorMsg;     //错误描述符
        int statusCode = 0;       //状态码
        bool streamFinish = false;//标记流式返回数据是否结束
        std::string fullResponse; //累积完整的响应(拼接上流式返回的数据)

        //创建请求对象
        httplib::Request req;
        req.method = "POST";
        req.path = "/v1/chat/completions";
        req.headers = headers;
        req.body = json_string;

        // 设置响应头处理器,检测send返回值，应答过后的响应
        req.response_handler = [&](const httplib::Response& response){
            statusCode = response.status;
            if(200 != statusCode){
                gotError = true;
                errorMsg = "HTTP Error:" + std::to_string(statusCode);
                return false; //终止请求
            }

            return true;      // 成功就继续接受数据
        };

        // 设置body数据接收处理器 -- 解析流式响应的每个块是数据
        req.content_receiver = [&](const char* data, size_t len, uint64_t offset,uint64_t totalLength){
            // 验证http请求头是否出错
            if(gotError)
            {
                return false;
            }

            // 追加新数据到缓冲区
            buffer.append(data, len);
            std::cout<<"buffer: "<<buffer<<std::endl;

            // 处理所有的流式响应数据块，数据块之间以\n\n分隔
            size_t pos = 0;
            while((pos = buffer.find("\n\n")) != std::string::npos){ // 单个数据块处理结束循环退出,等待模型返回下一个数据块
                // 截取当前找到的数据块
                std::string chunk = buffer.substr(0, pos);
                buffer.erase(0, pos+2); //移除已经处理的数据块

                // 解析该块响应数据中模型返回的有效数据
                // 处理空行和注释,以:开头的是注释行
                if(chunk.empty() || chunk[0] == ':'){
                    continue;
                }

                // 获取模型返回的有效数据
                if(chunk.compare(0,6,"data: ") == 0)
                {
                    std::string modelData = chunk.substr(6);

                    // 处理结束标记
                    if(modelData == "[DONE]")
                    {
                        callback("", true); // 数据流结束了，返回true
                        streamFinish = true;
                        return true;
                    }

                    // 反序列化
                    Json::Value modelDataJson;
                    Json::CharReaderBuilder reader;
                    std::string errors;
                    std::istringstream modelDataStream(modelData);
                    if(Json::parseFromStream(reader,modelDataStream,&modelDataJson,&errors))
                    {
                        // 模型返回的json格式的数据现在就保存在modelDataJson对象当中
                        if(modelDataJson.isMember("choices") &&
                            modelDataJson["choices"].isArray() &&
                            !modelDataJson["choices"].empty() &&
                            modelDataJson["choices"][0].isMember("delta")&&
                            modelDataJson["choices"][0]["delta"].isMember("content"))
                            {
                                std::string content = modelDataJson["choices"][0]["delta"]["content"].asString();
                                // 把结果拼接到fullResponse
                                fullResponse += content;

                                // 将本次解析出的模型返回的有效数据传给调用sendMessageStream函数的用户使用--callback()
                                callback(content,false); // 流式数据还没有完
                            }
                    }
                    else // 反序列化解析失败
                    {
                        WARN("DeepSeek SSE JSON parse error : {}", errors);
                    }

                }
                
            }

            return true; // 以上的操作都是在定义req请求对象：请求体，内容处理函数
        };

        // 给模型发送请求
        httplib::Result result = client.send(req);
        if(!result) // 有bool()重载
        {
            // 请求发送失败，出现网络问题，比如DNS解析失败，连接超时
            ERR("Netword error {}",to_string(result.error()));
            return "";
        }

        // 确保流式操作正常结束
        if(!streamFinish)
        {
            WARN("strean ended without [DONE] marker");
            callback("",true); // true表示结束
        }

        return fullResponse;
    }

} // namespace ai_chat_sdk