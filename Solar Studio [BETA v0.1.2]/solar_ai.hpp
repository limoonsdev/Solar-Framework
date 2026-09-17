#pragma once
#include <string>
#include <vector>

namespace Solar::Studio {

    class StudioApp;

    struct AIMessage {
        bool isUser = false;
        std::string content;
        std::vector<std::string> actionsExecuted;
        std::string timestamp;
    };

    struct AIResponse {
        std::string text;
        std::vector<std::string> actions;
        float confidence = 0.95f;
    };

    class SolarAI {
    public:
        static SolarAI& Get();

        void Initialize();

        // Process a natural language prompt and execute the requested actions on StudioApp
        AIResponse ProcessPrompt(const std::string& prompt, StudioApp& studio);

        // Get conversation history
        const std::vector<AIMessage>& GetHistory() const { return m_history; }
        void AddUserMessage(const std::string& text);
        void AddAIMessage(const std::string& text, const std::vector<std::string>& actions = {});
        void ClearHistory();

    private:
        SolarAI() = default;

        std::vector<AIMessage> m_history;
        std::string GetCurrentTimestamp();
    };

} // namespace Solar::Studio
