#pragma once
#include <string>

class WhisperAPI {
private:
    std::string apiKey;

public:
    explicit WhisperAPI(std::string api_key);
    
    std::string transcribeAudio(const std::string& file_path);
    std::string chatCompletion(const std::string& prompt, const std::string& system_role);
};