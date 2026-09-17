#pragma once
#include <string>
#include <vector>
#include <imgui.h>

namespace Solar::Tools {

    struct LayoutAnomaly {
        std::string componentId;
        std::string label;
        std::string issueType;
        float actualMargin;
        float requiredMargin;
        bool autoFixed;
    };

    class LayoutAuditor {
    public:
        static LayoutAuditor& Get();

        void AuditElement(const char* componentId, const char* label,
                          const ImVec2& elemMin, const ImVec2& elemMax,
                          const ImVec2& parentMin, const ImVec2& parentMax,
                          float minMargin = 8.0f);

        void RunAutomatedFullAudit();
        void ExportAuditLog(const char* filepath = "solar_layout_audit.log");

        const std::vector<LayoutAnomaly>& GetAnomalies() const { return m_anomalies; }
        int GetTotalElementsChecked() const { return m_checkedCount; }
        int GetFixedCount() const { return m_fixedCount; }

    private:
        LayoutAuditor() = default;
        std::vector<LayoutAnomaly> m_anomalies;
        int m_checkedCount = 0;
        int m_fixedCount = 0;
    };

} // namespace Solar::Tools
