#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "BotClient.hpp"
#include "WhisperAPI.hpp"
#include "CommandHandler.hpp"

class TelegramBot {
private:
    BotClient client;
    std::unique_ptr<WhisperAPI> openAI;
    std::unique_ptr<CommandHandler> handler;

    void processUpdates(const nlohmann::json& updates);

public:
    TelegramBot(const std::string& token);
    void init(std::string openai_key);
    void start();
};