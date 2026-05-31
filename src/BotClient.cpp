#include "BotClient.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <cpr/cpr.h>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

std::string BotClient::sendRequest(const std::string &method, const nlohmann::json &payload)
{
    std::string url = "https://api.telegram.org/bot" + bot_token + "/" + method;

    auto res = cpr::Post(
        cpr::Url{url},
        cpr::Body{payload.dump()},
        cpr::Header{{"Content-Type", "application/json"}});

    if (res.status_code != 200)
    {
        std::cerr << "[API ERROR]: " << res.status_code << " -> " << res.text << std::endl;
        return "";
    }

    return res.text;
}

#include <fstream>

bool BotClient::downloadFile(const std::string &file_path, const std::string &save_path)
{
    std::ofstream file(save_path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "[ПОМИЛКА]: Не вдалося створити файл для запису: " << save_path << std::endl;
        return false;
    }

    std::string url = "https://api.telegram.org/file/bot" + bot_token + "/" + file_path;

    auto write_func = [&file](std::string_view data, intptr_t userdata) -> bool
    {
        file.write(data.data(), data.size());
        return true;
    };

    auto res = cpr::Get(
        cpr::Url{url},
        cpr::WriteCallback{write_func});

    file.close();

    if (res.status_code == 200)
    {
        return true;
    }

    std::cerr << "[ПОМИЛКА ЗАВАНТАЖЕННЯ]: Статус " << res.status_code << " -> " << res.text << std::endl;
    return false;
}

int32_t BotClient::sendMessage(int64_t chat_id, const std::string &text, const nlohmann::json &reply_markup)
{
    nlohmann::json payload = {
        {"chat_id", chat_id},
        {"text", text},
        {"parse_mode", "Markdown"}};

    if (!reply_markup.is_null())
    {
        payload["reply_markup"] = reply_markup;
    }

auto raw_response = sendRequest("sendMessage", payload);
    
    try 
    {
        nlohmann::json response = nlohmann::json::parse(raw_response);

        if (response.contains("result") && response["result"].contains("message_id"))
        {
            return response["result"]["message_id"].get<int32_t>();
        }
    }
    catch (const nlohmann::json::parse_error& e) 
    {
        std::cerr << "Помилка парсингу відповіді Telegram: " << e.what() << std::endl;
    }

    return 0;
}

std::string BotClient::getFilePath(const std::string &file_id)
{
    std::string response_text = sendRequest("getFile", {{"file_id", file_id}});
    if (response_text.empty())
    {
        return "";
    }

    try
    {
        auto json_res = nlohmann::json::parse(response_text);
        if (json_res.contains("ok") && json_res["ok"].get<bool>())
        {
            return json_res["result"]["file_path"].get<std::string>();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "[JSON PARSE ERROR]: " << e.what() << std::endl;
    }

    return "";
}

BotClient::BotClient(std::string token) : bot_token(token) {}

void BotClient::deleteMessage(int64_t chat_id, int32_t message_id)
{
    nlohmann::json body = {
        {"chat_id", chat_id},
        {"message_id", message_id}};
    sendRequest("deleteMessage", body);
}

void BotClient::editMessageReplyMarkup(int64_t chat_id, int32_t message_id, const nlohmann::json &reply_markup)
{
    nlohmann::json body = {
        {"chat_id", chat_id},
        {"message_id", message_id},
        {"reply_markup", reply_markup}};
    sendRequest("editMessageReplyMarkup", body);
}