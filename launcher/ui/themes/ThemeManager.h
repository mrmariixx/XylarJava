#pragma once

#include <QDir>
#include <QLoggingCategory>
#include <QString>
#include <memory>

#include "IconTheme.h"
<<<<<<< HEAD
#include "ui/themes/ITheme.h"

inline auto themeDebugLog()
{
    return qDebug() << "[Theme]";
}
inline auto themeWarningLog()
{
=======
#include "ui/themes/CatPack.h"
#include "ui/themes/ITheme.h"

inline auto themeDebugLog() {
    return qDebug() << "[Theme]";
}
inline auto themeWarningLog() {
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    return qWarning() << "[Theme]";
}

class ThemeManager {
   public:
    ThemeManager();
    ~ThemeManager();

    QList<IconTheme*> getValidIconThemes();
    QList<ITheme*> getValidApplicationThemes();
    bool isValidIconTheme(const QString& id);
    bool isValidApplicationTheme(const QString& id);
    QDir getIconThemesFolder();
    QDir getApplicationThemesFolder();
<<<<<<< HEAD
=======
    QDir getCatPacksFolder();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    void applyCurrentlySelectedTheme(bool initial = false);
    void setIconTheme(const QString& name);
    void setApplicationTheme(const QString& name, bool initial = false);

<<<<<<< HEAD
    /// Optional left-side portrait/image for the current theme (themes/<id>/resources/side.png).
    QString getThemeSideImagePath() const;
=======
    /// @brief Returns the background based on selected and with events (Birthday, XMas, etc.)
    /// @param catName Optional, if you need a specific background.
    /// @return
    QString getCatPack(QString catName = "");
    QList<CatPack*> getValidCatPacks();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

    const LogColors& getLogColors() { return m_logColors; }

    void refresh();

   private:
    std::map<QString, std::unique_ptr<ITheme>> m_themes;
    std::map<QString, IconTheme> m_icons;
<<<<<<< HEAD
    QDir m_iconThemeFolder{ "iconthemes" };
    QDir m_applicationThemeFolder{ "themes" };
=======
    QDir m_iconThemeFolder{"iconthemes"};
    QDir m_applicationThemeFolder{"themes"};
    QDir m_catPacksFolder{"catpacks"};
    std::map<QString, std::unique_ptr<CatPack>> m_catPacks;
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QPalette m_defaultPalette;
    QString m_defaultStyle;
    LogColors m_logColors;

    void initializeThemes();
<<<<<<< HEAD
    QString addTheme(std::unique_ptr<ITheme> theme);
    ITheme* getTheme(QString themeId);
    QString addIconTheme(IconTheme theme);
    void initializeIcons();
    void initializeWidgets();
    void installBundledThemes();
    void loadThemesFromFolder(const QDir& folder, const QString& darkThemeId);
=======
    void initializeCatPacks();
    QString addTheme(std::unique_ptr<ITheme> theme);
    ITheme* getTheme(QString themeId);
    QString addIconTheme(IconTheme theme);
    QString addCatPack(std::unique_ptr<CatPack> catPack);
    void initializeIcons();
    void initializeWidgets();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

    // On non-Mac systems, this is a no-op.
    void setTitlebarColorOnMac(WId windowId, QColor color);
    // This also will set the titlebar color of newly opened windows after this method is called.
    // On non-Mac systems, this is a no-op.
    void setTitlebarColorOfAllWindowsOnMac(QColor color);
    // On non-Mac systems, this is a no-op.
    void stopSettingNewWindowColorsOnMac();
#ifdef Q_OS_MACOS
    NSObject* m_windowTitlebarObserver = nullptr;
#endif

<<<<<<< HEAD
    const QStringList builtinIcons{ "pe_colored", "pe_light", "pe_dark", "pe_blue",    "breeze_light", "breeze_dark",
                                    "OSX",        "iOS",      "flat",    "flat_white", "multimc" };
=======
    const QStringList builtinIcons{"pe_colored", "pe_light", "pe_dark", "pe_blue",    "breeze_light", "breeze_dark",
                                   "OSX",        "iOS",      "flat",    "flat_white", "multimc"};
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
};
