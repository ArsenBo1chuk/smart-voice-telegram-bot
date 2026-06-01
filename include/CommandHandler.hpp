#pragma once
#include <nlohmann/json.hpp>
#include "BotClient.hpp"
#include "WhisperAPI.hpp"
#include <unordered_map>

class CommandHandler
{
private:
    BotClient &client;
    WhisperAPI &openAI;

    void handleTextMessage(const nlohmann::json &message);
    void handleVoiceMessage(const nlohmann::json &message);
    void handleCallbackQuery(const nlohmann::json &callback_query);
    void handleVideoMessage(const nlohmann::json & message);
    void handleAudioMessage(const nlohmann::json & message);
    nlohmann::json getMediaKeyboard();
    struct VoiceCache
    {
        std::string file_id;
        std::string recognized_text; 
    };

    std::map<int64_t, VoiceCache> user_last_voice;

public:
    CommandHandler(BotClient &bot_client, WhisperAPI &openai_client);
    void handleUpdate(const nlohmann::json &update);
};