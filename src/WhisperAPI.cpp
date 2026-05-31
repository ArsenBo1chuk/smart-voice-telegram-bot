#include "WhisperAPI.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

WhisperAPI::WhisperAPI(std::string api_key) : apiKey(api_key) {}

std::string WhisperAPI::transcribeAudio(const std::string& file_path) 
{
    cpr::Multipart multipart_data{
        {"model", "whisper-1"},
        {"file", cpr::File{file_path}}
    };

    auto res = cpr::Post(
        cpr::Url{"https://api.openai.com/v1/audio/transcriptions"},
        cpr::Header{{"Authorization", "Bearer " + apiKey}},
        multipart_data
    );

    if (res.status_code == 200) {
        json j = json::parse(res.text);
        if (j.contains("text")) {
            return j["text"].get<std::string>();
        }
    }

    std::cerr << "[OPENAI ERROR]: " << res.text << std::endl;
    return "";
}

std::string WhisperAPI::chatCompletion(const std::string& prompt, const std::string& system_role) 
{
    json body = {
        {"model", "gpt-4o-mini"},
        {"messages", json::array({
            {{"role", "system"}, {"content", system_role}},
            {{"role", "user"}, {"content", prompt}}
        })}
    };

    auto res = cpr::Post(
        cpr::Url{"https://api.openai.com/v1/chat/completions"},
        cpr::Header{
            {"Authorization", "Bearer " + apiKey},
            {"Content-Type", "application/json"}
        },
        cpr::Body{body.dump()}
    );

    if (res.status_code == 200) {
        json j = json::parse(res.text);
        return j["choices"][0]["message"]["content"].get<std::string>();
    }

    std::cerr << "[OPENAI ERROR]: " << res.text << std::endl;
    return "";
}