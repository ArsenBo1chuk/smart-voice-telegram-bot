#pragma once
#include <string>
#include <nlohmann/json.hpp>

class BotClient 
{
private:
    std::string bot_token; 

public:
    bool downloadFile(const std::string& file_path, const std::string& save_path);
    int32_t sendMessage(int64_t chat_id, const std::string& text, const nlohmann::json& reply_markup = nullptr);
    void editMessageReplyMarkup(int64_t chat_id, int32_t message_id, const nlohmann::json &reply_markup = nlohmann::json::object());
    void deleteMessage(int64_t chat_id, int32_t message_id);
    std::string getFilePath(const std::string& file_id);
    std::string sendRequest(const std::string& method, const nlohmann::json& payload);

    BotClient(std::string bot_token);
};