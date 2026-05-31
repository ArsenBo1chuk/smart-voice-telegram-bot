#include "TelegramBot.hpp"
#include <iostream>
#include <thread>
#include <chrono>


TelegramBot::TelegramBot(const std::string& token) : client(token) {}

void TelegramBot::init(std::string openai_key) 
{
    openAI = std::make_unique<WhisperAPI>(openai_key);
    handler = std::make_unique<CommandHandler>(client, *openAI);
    
    std::cout << "[ОК] Компоненти бота успішно ініціалізовані!" << std::endl;
}

void TelegramBot::start() 
{
    std::cout << "[СТАРТ] Бот запущений і починає довге опитування (Long Polling)..." << std::endl;
    
    int64_t offset = 0;

    while (true) {
        try {
            nlohmann::json payload = {
                {"offset", offset},
                {"timeout", 30} 
            };

            std::string response = client.sendRequest("getUpdates", payload);
            
            if (!response.empty()) {
                auto json_res = nlohmann::json::parse(response);
                
                if (json_res.contains("ok") && json_res["ok"].get<bool>()) {
                    auto updates = json_res["result"];
                    
                    if (!updates.empty()) {
                        offset = updates.back()["update_id"].get<int64_t>() + 1;
                        processUpdates(updates);
                    }
                }
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "[ПОМИЛКА ЦИКЛУ]: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void TelegramBot::processUpdates(const nlohmann::json& updates) 
{
    for (const auto& update : updates) {
        if (handler) {
            handler->handleUpdate(update);
        }
    }
}