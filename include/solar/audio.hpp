#pragma once

namespace Solar {

    class AudioSystem {
    public:
        static AudioSystem& Get();

        void Initialize();
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        void PlayClick();
        void PlayToggle(bool state);
        void PlayNotification();
        void PlayError();
        void PlayTabSwitch();

    private:
        AudioSystem();
        bool m_enabled = true;
    };

    namespace Audio {
        inline void PlayClick() { AudioSystem::Get().PlayClick(); }
        inline void PlayToggle(bool state) { AudioSystem::Get().PlayToggle(state); }
        inline void PlayNotification() { AudioSystem::Get().PlayNotification(); }
        inline void PlayError() { AudioSystem::Get().PlayError(); }
        inline void PlayTabSwitch() { AudioSystem::Get().PlayTabSwitch(); }
        inline void SetEnabled(bool enabled) { AudioSystem::Get().SetEnabled(enabled); }
        inline bool IsEnabled() { return AudioSystem::Get().IsEnabled(); }
    }

} // namespace Solar
