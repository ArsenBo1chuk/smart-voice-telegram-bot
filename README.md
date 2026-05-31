# 🤖 Smart Voice Telegram Bot

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.15+-green)
![Telegram](https://img.shields.io/badge/Telegram-Bot_API-2CA5E0)
![OpenAI](https://img.shields.io/badge/OpenAI-Whisper%20%7C%20GPT-black)
![License](https://img.shields.io/badge/License-MIT-yellow)

An intelligent Telegram bot written in modern C++ that transcribes voice messages using OpenAI Whisper and performs AI-powered text processing using ChatGPT.

---

## ✨ Features

| Feature | Description |
|----------|-------------|
| 🎤 Speech-to-Text | Convert Telegram voice messages into text |
| 📋 Smart Summary | Generate concise summaries |
| 🏷️ Hashtag Generation | Create relevant hashtags automatically |
| 🌍 Translation | Translate text into multiple languages |
| ⚡ Runtime Cache | Avoid repeated transcription requests |
| 📱 Inline Keyboard | Convenient Telegram UI |
| 🤖 AI Processing | Powered by OpenAI models |

---

## 🌍 Supported Languages

| Language | Code |
|-----------|------|
| 🇺🇸 English | EN |
| 🇵🇱 Polish | PL |
| 🇩🇪 German | DE |
| 🇫🇷 French | FR |

---

## 🏗️ Architecture

```text
User
 │
 ▼
Telegram
 │
 ▼
Telegram Bot API
 │
 ▼
BotClient
 │
 ▼
CommandHandler
 │
 ├─────────────┬──────────────┐
 ▼             ▼              ▼
WhisperAPI  TextAnalyzer   Cache
 │             │
 ▼             ▼
OpenAI     ChatGPT
```

---

## 📁 Project Structure

```text
TELEGRAM_BOT/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── BotClient.hpp
│   ├── CommandHandler.hpp
│   ├── TelegramBot.hpp
│   ├── TextAnalyzer.hpp
│   └── WhisperAPI.hpp
│
└── src/
    ├── BotClient.cpp
    ├── CommandHandler.cpp
    ├── TelegramBot.cpp
    ├── TextAnalyzer.cpp
    ├── WhisperAPI.cpp
    └── main.cpp
```

---

## ⚙️ Requirements

- CMake 3.15+
- C++17 Compiler
- OpenAI API Key
- Telegram Bot Token
- Internet Connection

---

## 🚀 Installation

### Clone Repository

```bash
git clone https://github.com/ArsenBo1chuk/TELEGRAM_BOT.git
cd TELEGRAM_BOT
```

### Configure API Keys

To keep your tokens secure and separated from the repository, the bot loads them via a local configuration file. Create a file named config.json in your project root or execution directory:

```json
{
    "telegram_bot_token": "YOUR_ACTUAL_TELEGRAM_BOT_TOKEN",
    "openai_api_key": "YOUR_ACTUAL_OPENAI_API_KEY"
}
```

### Build Project

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

### Run

```bash
./telegram_bot
```

---

## 🔄 Bot Workflow

```text
User sends voice message
          │
          ▼
Audio downloaded from Telegram
          │
          ▼
Whisper transcription
          │
          ▼
Text stored in cache
          │
          ▼
Choose action:
 ├─ Summary
 ├─ Hashtags
 └─ Translation
```

---

## ⚡ Smart Cache System

The bot performs speech recognition only once.

Instead of sending the same audio to Whisper multiple times:

```text
Voice Message
      │
      ▼
Transcription
      │
      ▼
RAM Cache
      │
      ├─ Summary
      ├─ Hashtags
      └─ Translation
```

Benefits:

- Faster responses
- Lower API usage
- Reduced OpenAI costs

---

## 🛣️ Roadmap

- [x] Voice transcription
- [x] Summaries
- [x] Hashtags
- [x] Translation
- [x] Runtime cache
- [ ] Database support
- [ ] Docker deployment
- [ ] User statistics
- [ ] PDF export
- [ ] Text-to-Speech
- [ ] Admin panel

---

## 🤝 Contributing

Contributions are welcome.

1. Fork the repository
2. Create a feature branch

```bash
git checkout -b feature/my-feature
```

3. Commit changes

```bash
git commit -m "Add new feature"
```

4. Push branch

```bash
git push origin feature/my-feature
```

5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License.

---

## 👨‍💻 Author

**Arsen**

C++ Developer

GitHub:
https://github.com/ArsenBo1chuk

---

⭐ If you like this project, consider giving it a star.