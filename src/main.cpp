#include <iostream>
#include <exception>
#include "TelegramBot.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

int main() 
{
    const std::string config_path = "config.json";
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) 
    {
        std::cerr << "Помилка: Не вдалося відкрити файл конфігурації '" << config_path << "'!\n";
        std::cerr << "Переконайтеся, що файл створено в робочій директорії бота.\n";
        return 1;
    }

    nlohmann::json config;
    try 
    {
        config_file >> config;
    }
    catch (const nlohmann::json::parse_error& e) 
    {
        std::cerr << "🛑 Помилка синтаксису JSON у файлі '" << config_path << "': " << e.what() << "\n";
        return 1;
    }

    if (!config.contains("telegram_bot_token") || !config.contains("openai_api_key")) 
    {
        std::cerr << "🛑 Помилка: У файлі конфігурації відсутні необхідні ключі!\n";
        std::cerr << "Перевірте наявність 'telegram_bot_token' та 'openai_api_key'.\n";
        return 1;
    }

    std::string bot_token = config["telegram_bot_token"].get<std::string>();
    std::string openai_key = config["openai_api_key"].get<std::string>();

    if (bot_token.empty() || openai_key.empty()) 
    {
        std::cerr << "Помилка: Токени в конфігураційному файлі не можуть бути пустими!\n";
        return 1;
    }
    try {
        TelegramBot bot(bot_token);
        std::string openAIKey = openai_key;
        
        bot.init(openAIKey);
        bot.start();
    } 
    catch (const std::exception& e) {
        std::cerr << "[КРИТИЧНА ПОМИЛКА]: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}




