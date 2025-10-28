#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <QWidget>
#include <QLineEdit>

// Qt FS
#include <QDir>
#include <QFileInfoList>
#endif

namespace CustomCppToolGem
{
    class CustomCppToolGemWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit CustomCppToolGemWidget(QWidget* parent = nullptr);
    
    protected:
        // Drag & Drop overrides
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;

    private:
        AZ::Data::AssetId m_matAssetID;

        QLineEdit* m_pathEdit = nullptr;

        void GenerateCubeEntityAtOrigin();
        
        

    private Q_SLOTS:
        void OnGenerateClicked();
        void OnPathEntered();  
        
    };
}
