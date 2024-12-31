#pragma once

#include "WindowBase.h"

namespace BHive
{
    class AssetEditor : public WindowBase
    {
    public:
        AssetEditor();

        virtual ~AssetEditor() = default;

        void OnUpdateContent();

        bool IsOpened() const { return mIsOpened; }

        virtual void SetContext(const UUID &handle) {};

    protected:
        virtual bool OnSave(const std::filesystem::path &) const { return false; };

        virtual bool OnLoad(const std::filesystem::path &) { return false; };

        virtual void OnMenuBar() {};

        virtual void OnWindowRender() = 0;

        virtual const char *GetFileDialogFilter() = 0;

    protected:
        std::filesystem::path mCurrentSavePath;

    private:
        void MenuBar();

    private:
        bool mIsOpened = true;
    };
} // namespace BHive
