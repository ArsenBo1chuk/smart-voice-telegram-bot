#include "CommandHandler.hpp"
#include "TextAnalyzer.hpp"
#include <iostream>
#include <cstdio>

CommandHandler::CommandHandler(BotClient &bot_client, WhisperAPI &openai_client)
    : client(bot_client), openAI(openai_client) {}

void CommandHandler::handleUpdate(const nlohmann::json &update)
{
    if (update.contains("message"))
    {
        auto message = update["message"];

        if (message.contains("voice"))
        {
            handleVoiceMessage(message);
        }
        else if (message.contains("text"))
        {
            handleTextMessage(message);
        }
    }
    else if (update.contains("callback_query"))
    {
        handleCallbackQuery(update["callback_query"]);
    }
}

void CommandHandler::handleTextMessage(const nlohmann::json &message)
{
    int64_t chat_id = message["chat"]["id"].get<int64_t>();
    std::string text = message["text"].get<std::string>();

    if (text == "/start")
    {
        std::string welcome_text =
            "👋 Привіт! Я твій інтелектуальний ШІ-асистент. 🤖✨\n\n"
            "🎙️ Просто надішли мені голосове повідомлення, "
            "і вибери дію, яку необхідно виконати!";

        client.sendMessage(chat_id, welcome_text);
    }
    else
    {
        client.sendMessage(chat_id, " 💬 Ти написав: " + text);
    }
}

void CommandHandler::handleVoiceMessage(const nlohmann::json &message)
{
    int64_t chat_id = message["chat"]["id"].get<int64_t>();
    std::string file_id = message["voice"]["file_id"].get<std::string>();

    user_last_voice[chat_id] = {file_id, ""};

    nlohmann::json inline_keyboard = {
        {"inline_keyboard", {{{{"text", "📝 Аудіо в текст"}, {"callback_data", "txt"}}, {{"text", "📋 Короткий опис"}, {"callback_data", "sum"}}}, {{{"text", "🏷️ Хештеги"}, {"callback_data", "tag"}}, {{"text", "🌐 Перекласти текст"}, {"callback_data", "transl"}}}}}};

    client.sendMessage(chat_id, "📥 Отримав голосове повідомлення! Що ти хочеш, щоб я з ним зробив? 👇", inline_keyboard);
}

void CommandHandler::handleCallbackQuery(const nlohmann::json &callback_query)
{
    int64_t chat_id = callback_query["message"]["chat"]["id"].get<int64_t>();
    std::string action = callback_query["data"].get<std::string>();
    std::string callback_id = callback_query["id"];
    int32_t message_id = callback_query["message"]["message_id"].get<int32_t>();

    if (user_last_voice.find(chat_id) == user_last_voice.end())
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});
        client.sendMessage(chat_id, "⚠️ Помилка: Я не пам'ятаю голосове повідомлення. Надішли його ще раз.");
        return;
    }

    auto &voice_cache = user_last_voice[chat_id];

    if (action == "transl")
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});

        nlohmann::json inline_keyboard = {
            {"inline_keyboard", {{{{"text", "🇺🇸 Англійська"}, {"callback_data", "Eng"}}, {{"text", "🇵🇱 Польська"}, {"callback_data", "Pol"}}}, {{{"text", "🇩🇪 Німецька"}, {"callback_data", "Nim"}}, {{"text", "🇫🇷 Французька"}, {"callback_data", "Fran"}}}}}};

        client.sendMessage(chat_id, "Виберіть на яку мову перекласти текст аудіо: 👇", inline_keyboard);
        return;
    }

    std::string recognized_text = voice_cache.recognized_text;
    int32_t status_msg_id = 0;

    if (recognized_text.empty())
    {
        status_msg_id = client.sendMessage(chat_id, "⏳ Завантажую та обробляю аудіофайл...");

        std::string path = client.getFilePath(voice_cache.file_id);
        if (path.empty())
        {
            client.deleteMessage(chat_id, status_msg_id);
            client.sendMessage(chat_id, "⚠️ Не вдалося отримати файл від Telegram.");
            return;
        }

        std::string filename = "voice_" + std::to_string(chat_id) + ".ogg";
        if (!client.downloadFile(path, filename))
        {
            client.deleteMessage(chat_id, status_msg_id);
            client.sendMessage(chat_id, "⚠️ Помилка завантаження файлу.");
            return;
        }

        recognized_text = openAI.transcribeAudio(filename); 
        std::remove(filename.c_str());

        if (recognized_text.empty())
        {
            client.deleteMessage(chat_id, status_msg_id);
            client.sendMessage(chat_id, "⚠️ Не вдалося розпізнати аудіо в текст.");
            return;
        }

        voice_cache.recognized_text = recognized_text; 
    }

    TextAnalyzer analyzer;

    if (action == "txt")
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});

        if (status_msg_id != 0) {
            client.deleteMessage(chat_id, status_msg_id);
        }

        client.sendMessage(chat_id, "📝 **Розпізнаний текст:**\n\n" + recognized_text);
    }
    else if (action == "sum")
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});

        if (status_msg_id != 0) {
            client.deleteMessage(chat_id, status_msg_id);
        }
        
        int32_t msg_id = client.sendMessage(chat_id, "⏳ Складаю короткий зміст...");

        std::string prompt = "Ти — крутий редактор, який вміє вичіпляти найголовніше. Зроби стислий, живий та місткий підсумок цього тексту українською мовою."
                             "Важливо : Пиши простою людською мовою,"
                             "без офіціозу.Уникай фраз на кшталт 'автор пише', 'у тексті йдеться про', 'на думку автора'.Переходь одразу до суті, ніби переказуєш головні тези другові в месенджері.\n" +
                             recognized_text;

        std::string processed_prompt = analyzer.analyze(prompt);
        std::string result = openAI.chatCompletion(processed_prompt, "Ти корисний асистент.");

        client.deleteMessage(chat_id, msg_id);
        client.sendMessage(chat_id, "📋 **Короткий опис:**\n\n" + result);
    }
    else if (action == "tag")
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});

        if (status_msg_id != 0) {
            client.deleteMessage(chat_id, status_msg_id);
        }
        
        int32_t msg_id = client.sendMessage(chat_id, "⏳ Генерую хештеги...");

        std::string prompt = "Прочитай текст і згенеруй список релевантних хештегів українською мовою через пробіл:\n" + recognized_text;

        std::string processed_prompt = analyzer.analyze(prompt);
        std::string result = openAI.chatCompletion(processed_prompt, "Ти експерт з тегування. Повертай тільки хештеги.");

        client.deleteMessage(chat_id, msg_id);
        client.sendMessage(chat_id, "🏷️ **Хештеги для тексту:**\n\n" + result);
    }
    else if (action == "Eng" || action == "Pol" || action == "Nim" || action == "Fran")
    {
        client.sendRequest("answerCallbackQuery", {{"callback_query_id", callback_id}});

        client.deleteMessage(chat_id, message_id);

        if (status_msg_id != 0) {
            client.deleteMessage(chat_id, status_msg_id);
        }

        std::string target_language = "";
        std::string flag = "";
        if (action == "Eng")       { target_language = "англійську"; flag = "🇺🇸"; }
        else if (action == "Pol")  { target_language = "польську";   flag = "🇵🇱"; }
        else if (action == "Nim")  { target_language = "німецьку";   flag = "🇩🇪"; }
        else if (action == "Fran") { target_language = "французьку"; flag = "🇫🇷"; }

        int32_t msg_id = client.sendMessage(chat_id, "🔄 Перекладаю на " + target_language + "...");

        std::string prompt = "Прочитай текст і переклади на " + target_language + " мову, просто дай мені перекладений текст і все і більше нічого:\n" + recognized_text;
        std::string result = openAI.chatCompletion(prompt, "Ти професійний перекладач");

        client.deleteMessage(chat_id, msg_id);
        client.sendMessage(chat_id, flag + " **Переклад:**\n\n" + result);
    }
}