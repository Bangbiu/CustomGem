#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <QWidget>
#endif

namespace CustomCppToolGem
{
    class CustomCppToolGemWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit CustomCppToolGemWidget(QWidget* parent = nullptr);

    private Q_SLOTS:
        void OnGenerateClicked();

    private:
        void GenerateCubeEntityAtOrigin();
    };
}
