#include "ThemeManager.h"

#include <QApplication>
<<<<<<< HEAD
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QStyle>
#include <QStyleFactory>

#include "Exception.h"
#include "FileSystem.h"
#include "ui/themes/BrightTheme.h"
=======
#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QImageReader>
#include <QStyle>
#include <QStyleFactory>
#include "Exception.h"
#include "ui/themes/BrightTheme.h"
#include "ui/themes/CatPack.h"
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
#include "ui/themes/CustomTheme.h"
#include "ui/themes/DarkTheme.h"
#include "ui/themes/SystemTheme.h"
#include "ui/themes/Win7AeroTheme.h"

#include "Application.h"
#include "settings/SettingsObject.h"

ThemeManager::ThemeManager()
{
    QIcon::setFallbackThemeName(QIcon::themeName());
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << m_iconThemeFolder.path());

    themeDebugLog() << "Determining System Widget Theme...";
    const auto& style = QApplication::style();
    m_defaultStyle = style->objectName();
    themeDebugLog() << "System theme seems to be:" << m_defaultStyle;

    m_defaultPalette = QApplication::palette();

    initializeThemes();
<<<<<<< HEAD
=======
    initializeCatPacks();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
}

ThemeManager::~ThemeManager()
{
    stopSettingNewWindowColorsOnMac();
}

<<<<<<< HEAD
=======
/// @brief Adds the Theme to the list of themes
/// @param theme The Theme to add
/// @return Theme ID
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
QString ThemeManager::addTheme(std::unique_ptr<ITheme> theme)
{
    QString id = theme->id();
    if (m_themes.find(id) == m_themes.end())
        m_themes.emplace(id, std::move(theme));
    else
        themeWarningLog() << "Theme(" << id << ") not added to prevent id duplication";
    return id;
}

<<<<<<< HEAD
=======
/// @brief Gets the Theme from the List via ID
/// @param themeId Theme ID of theme to fetch
/// @return Theme at themeId
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
ITheme* ThemeManager::getTheme(QString themeId)
{
    return m_themes[themeId].get();
}

QString ThemeManager::addIconTheme(IconTheme theme)
{
    QString id = theme.id();
    if (m_icons.find(id) == m_icons.end())
        m_icons.emplace(id, std::move(theme));
    else
        themeWarningLog() << "IconTheme(" << id << ") not added to prevent id duplication";
    return id;
}

void ThemeManager::initializeThemes()
{
<<<<<<< HEAD
    initializeIcons();
=======
    // Icon themes
    initializeIcons();

    // Initialize widget themes
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    initializeWidgets();
}

void ThemeManager::initializeIcons()
{
<<<<<<< HEAD
=======
    // TODO: icon themes and instance icons do not mesh well together. Rearrange and fix discrepancies!
    // set icon theme search path!
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    themeDebugLog() << "<> Initializing Icon Themes";

    for (const QString& id : builtinIcons) {
        IconTheme theme(id, QString(":/icons/%1").arg(id));
        if (!theme.load()) {
            themeWarningLog() << "Couldn't load built-in icon theme" << id;
            continue;
        }

        addIconTheme(std::move(theme));
        themeDebugLog() << "Loaded Built-In Icon Theme" << id;
    }

    if (!m_iconThemeFolder.mkpath("."))
        themeWarningLog() << "Couldn't create icon theme folder";
    themeDebugLog() << "Icon Theme Folder Path:" << m_iconThemeFolder.absolutePath();

    QDirIterator directoryIterator(m_iconThemeFolder.path(), QDir::Dirs | QDir::NoDotAndDotDot);
    while (directoryIterator.hasNext()) {
        QDir dir(directoryIterator.next());
        IconTheme theme(dir.dirName(), dir.path());
        if (!theme.load())
            continue;

        addIconTheme(std::move(theme));
        themeDebugLog() << "Loaded Custom Icon Theme from" << dir.path();
    }

    themeDebugLog() << "<> Icon themes initialized.";
}

<<<<<<< HEAD
void ThemeManager::installBundledThemes()
{
    const QDir bundled(QDir(QCoreApplication::applicationDirPath()).filePath("themes"));
    if (!bundled.exists()) {
        themeDebugLog() << "No bundled themes folder next to the executable.";
        return;
    }

    if (!m_applicationThemeFolder.mkpath(".")) {
        themeWarningLog() << "Couldn't create theme folder for bundled install";
        return;
    }

    QDirIterator directoryIterator(bundled.path(), QDir::Dirs | QDir::NoDotAndDotDot);
    while (directoryIterator.hasNext()) {
        const QDir srcDir(directoryIterator.next());
        const QString id = srcDir.dirName();
        // Skip the old cat theme if somehow present next to the binary
        if (id.compare("Cat", Qt::CaseInsensitive) == 0)
            continue;

        const QString destPath = m_applicationThemeFolder.absoluteFilePath(id);
        if (QDir(destPath).exists())
            continue;

        themeDebugLog() << "Installing bundled theme:" << id;
        try {
            FS::copy(srcDir.absolutePath(), destPath)();
        } catch (const Exception& e) {
            themeWarningLog() << "Failed to install bundled theme" << id << ":" << e.cause();
        }
    }
}

void ThemeManager::loadThemesFromFolder(const QDir& folder, const QString& darkThemeId)
{
    QDirIterator directoryIterator(folder.path(), QDir::Dirs | QDir::NoDotAndDotDot);
    while (directoryIterator.hasNext()) {
        QDir dir(directoryIterator.next());
        if (dir.dirName().compare("Cat", Qt::CaseInsensitive) == 0)
            continue;

        QFileInfo themeJson(dir.absoluteFilePath("theme.json"));
        if (themeJson.exists()) {
            themeDebugLog() << "Loading JSON Theme from:" << themeJson.absoluteFilePath();
            addTheme(std::make_unique<CustomTheme>(getTheme(darkThemeId), themeJson, true));
        } else {
            QDirIterator stylesheetFileIterator(dir.absoluteFilePath(""), { "*.qss", "*.css" }, QDir::Files);
            while (stylesheetFileIterator.hasNext()) {
                QFile customThemeFile(stylesheetFileIterator.next());
                QFileInfo customThemeFileInfo(customThemeFile);
                themeDebugLog() << "Loading QSS Theme from:" << customThemeFileInfo.absoluteFilePath();
                addTheme(std::make_unique<CustomTheme>(getTheme(darkThemeId), customThemeFileInfo, false));
            }
        }
    }
}

=======
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
void ThemeManager::initializeWidgets()
{
    themeDebugLog() << "<> Initializing Widget Themes";
    themeDebugLog() << "Loading Built-in Theme:" << addTheme(std::make_unique<SystemTheme>(m_defaultStyle, m_defaultPalette, true));
    auto darkThemeId = addTheme(std::make_unique<DarkTheme>());
    themeDebugLog() << "Loading Built-in Theme:" << darkThemeId;
    themeDebugLog() << "Loading Built-in Theme:" << addTheme(std::make_unique<BrightTheme>());
    themeDebugLog() << "Loading Built-in Theme:" << addTheme(std::make_unique<Win7AeroTheme>());

    themeDebugLog() << "<> Initializing System Widget Themes";
    QStringList styles = QStyleFactory::keys();
    for (auto& st : styles) {
#ifdef Q_OS_WINDOWS
        if (QSysInfo::productVersion() != "11" && st == "windows11") {
            continue;
        }
#endif
        themeDebugLog() << "Loading System Theme:" << addTheme(std::make_unique<SystemTheme>(st, m_defaultPalette, false));
    }

<<<<<<< HEAD
=======
    // TODO: need some way to differentiate same name themes in different subdirectories
    //  (maybe smaller grey text next to theme name in dropdown?)

    // Built-in themes bundled from PrismLauncher/Themes (https://github.com/PrismLauncher/Themes).
    // These ship inside the application itself, so they show up in Settings > Appearance > Theme
    // without the user having to install anything manually.
    themeDebugLog() << "<> Initializing Bundled Themes";
    QDirIterator bundledThemeIterator(":/themes", QDir::Dirs | QDir::NoDotAndDotDot);
    while (bundledThemeIterator.hasNext()) {
        QDir dir(bundledThemeIterator.next());
        QFileInfo themeJson(dir.absoluteFilePath("theme.json"));
        if (themeJson.exists()) {
            themeDebugLog() << "Loading Bundled Theme from:" << themeJson.absoluteFilePath();
            addTheme(std::make_unique<CustomTheme>(getTheme(darkThemeId), themeJson, true));
        }
    }

>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    if (!m_applicationThemeFolder.mkpath("."))
        themeWarningLog() << "Couldn't create theme folder";
    themeDebugLog() << "Theme Folder Path:" << m_applicationThemeFolder.absolutePath();

<<<<<<< HEAD
    installBundledThemes();
    loadThemesFromFolder(m_applicationThemeFolder, darkThemeId);
=======
    QDirIterator directoryIterator(m_applicationThemeFolder.path(), QDir::Dirs | QDir::NoDotAndDotDot);
    while (directoryIterator.hasNext()) {
        QDir dir(directoryIterator.next());
        QFileInfo themeJson(dir.absoluteFilePath("theme.json"));
        if (themeJson.exists()) {
            // Load "theme.json" based themes
            themeDebugLog() << "Loading JSON Theme from:" << themeJson.absoluteFilePath();
            addTheme(std::make_unique<CustomTheme>(getTheme(darkThemeId), themeJson, true));
        } else {
            // Load pure QSS Themes
            QDirIterator stylesheetFileIterator(dir.absoluteFilePath(""), { "*.qss", "*.css" }, QDir::Files);
            while (stylesheetFileIterator.hasNext()) {
                QFile customThemeFile(stylesheetFileIterator.next());
                QFileInfo customThemeFileInfo(customThemeFile);
                themeDebugLog() << "Loading QSS Theme from:" << customThemeFileInfo.absoluteFilePath();
                addTheme(std::make_unique<CustomTheme>(getTheme(darkThemeId), customThemeFileInfo, false));
            }
        }
    }
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

    themeDebugLog() << "<> Widget themes initialized.";
}

#ifndef Q_OS_MACOS
void ThemeManager::setTitlebarColorOnMac(WId windowId, QColor color) {}
void ThemeManager::setTitlebarColorOfAllWindowsOnMac(QColor color) {}
void ThemeManager::stopSettingNewWindowColorsOnMac() {}
#endif

QList<IconTheme*> ThemeManager::getValidIconThemes()
{
    QList<IconTheme*> ret;
    ret.reserve(m_icons.size());
    for (auto&& [id, theme] : m_icons) {
        ret.append(&theme);
    }
    return ret;
}

QList<ITheme*> ThemeManager::getValidApplicationThemes()
{
    QList<ITheme*> ret;
    ret.reserve(m_themes.size());
    for (auto&& [id, theme] : m_themes) {
        ret.append(theme.get());
    }
    return ret;
}

<<<<<<< HEAD
=======
QList<CatPack*> ThemeManager::getValidCatPacks()
{
    QList<CatPack*> ret;
    ret.reserve(m_catPacks.size());
    for (auto&& [id, theme] : m_catPacks) {
        ret.append(theme.get());
    }
    return ret;
}

>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
bool ThemeManager::isValidIconTheme(const QString& id)
{
    return !id.isEmpty() && m_icons.find(id) != m_icons.end();
}

bool ThemeManager::isValidApplicationTheme(const QString& id)
{
    return !id.isEmpty() && m_themes.find(id) != m_themes.end();
}

QDir ThemeManager::getIconThemesFolder()
{
    return m_iconThemeFolder;
}

QDir ThemeManager::getApplicationThemesFolder()
{
    return m_applicationThemeFolder;
}

<<<<<<< HEAD
=======
QDir ThemeManager::getCatPacksFolder()
{
    return m_catPacksFolder;
}

>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
void ThemeManager::setIconTheme(const QString& name)
{
    if (m_icons.find(name) == m_icons.end()) {
        themeWarningLog() << "Tried to set invalid icon theme:" << name;
        return;
    }

    QIcon::setThemeName(name);
}

void ThemeManager::setApplicationTheme(const QString& name, bool initial)
{
<<<<<<< HEAD
=======
    auto systemPalette = qApp->palette();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    auto themeIter = m_themes.find(name);
    if (themeIter != m_themes.end()) {
        auto& theme = themeIter->second;
        themeDebugLog() << "applying theme" << theme->name();
        theme->apply(initial);
        setTitlebarColorOfAllWindowsOnMac(qApp->palette().window().color());

        m_logColors = theme->logColorScheme();
    } else {
        themeWarningLog() << "Tried to set invalid theme:" << name;
    }
}

void ThemeManager::applyCurrentlySelectedTheme(bool initial)
{
    auto settings = APPLICATION->settings();
    setIconTheme(settings->get("IconTheme").toString());
    themeDebugLog() << "<> Icon theme set.";
    auto applicationTheme = settings->get("ApplicationTheme").toString();
    if (applicationTheme == "") {
        applicationTheme = m_defaultStyle;
    }
    setApplicationTheme(applicationTheme, initial);
    themeDebugLog() << "<> Application theme set.";
}

<<<<<<< HEAD
QString ThemeManager::getThemeSideImagePath() const
{
    const QStringList searchPaths = QDir::searchPaths("theme");
    for (const QString& path : searchPaths) {
        const QString side = QDir(path).filePath("side.png");
        if (QFileInfo::exists(side))
            return side;
        const QString sideJpg = QDir(path).filePath("side.jpg");
        if (QFileInfo::exists(sideJpg))
            return sideJpg;
    }
    return {};
=======
QString ThemeManager::getCatPack(QString catName)
{
    auto catIter = m_catPacks.find(!catName.isEmpty() ? catName : APPLICATION->settings()->get("BackgroundCat").toString());
    if (catIter != m_catPacks.end()) {
        auto& catPack = catIter->second;
        themeDebugLog() << "applying catpack" << catPack->id();
        return catPack->path();
    } else {
        themeWarningLog() << "Tried to get invalid catPack:" << catName;
    }

    return m_catPacks.begin()->second->path();
}

QString ThemeManager::addCatPack(std::unique_ptr<CatPack> catPack)
{
    QString id = catPack->id();
    if (m_catPacks.find(id) == m_catPacks.end())
        m_catPacks.emplace(id, std::move(catPack));
    else
        themeWarningLog() << "CatPack(" << id << ") not added to prevent id duplication";
    return id;
}

void ThemeManager::initializeCatPacks()
{
    QList<std::pair<QString, QString>> defaultCats{ { "kitteh", QObject::tr("Background Cat (from MultiMC)") },
                                                    { "rory", QObject::tr("Rory ID 11 (drawn by Ashtaka)") },
                                                    { "rory-flat", QObject::tr("Rory ID 11 (flat edition, drawn by Ashtaka)") },
                                                    { "teawie", QObject::tr("Teawie (drawn by SympathyTea)") } };
    for (auto [id, name] : defaultCats) {
        addCatPack(std::unique_ptr<CatPack>(new BasicCatPack(id, name)));
    }
    if (!m_catPacksFolder.mkpath("."))
        themeWarningLog() << "Couldn't create catpacks folder";
    themeDebugLog() << "CatPacks Folder Path:" << m_catPacksFolder.absolutePath();

    QStringList supportedImageFormats;
    for (auto format : QImageReader::supportedImageFormats()) {
        supportedImageFormats.append("*." + format);
    }
    auto loadFiles = [this, supportedImageFormats](QDir dir) {
        // Load image files directly
        QDirIterator ImageFileIterator(dir.absoluteFilePath(""), supportedImageFormats, QDir::Files);
        while (ImageFileIterator.hasNext()) {
            QFile customCatFile(ImageFileIterator.next());
            QFileInfo customCatFileInfo(customCatFile);
            themeDebugLog() << "Loading CatPack from:" << customCatFileInfo.absoluteFilePath();
            addCatPack(std::unique_ptr<CatPack>(new FileCatPack(customCatFileInfo)));
        }
    };

    loadFiles(m_catPacksFolder);

    QDirIterator directoryIterator(m_catPacksFolder.path(), QDir::Dirs | QDir::NoDotAndDotDot);
    while (directoryIterator.hasNext()) {
        QDir dir(directoryIterator.next());
        QFileInfo manifest(dir.absoluteFilePath("catpack.json"));
        if (manifest.isFile()) {
            try {
                // Load background manifest
                themeDebugLog() << "Loading background manifest from:" << manifest.absoluteFilePath();
                addCatPack(std::unique_ptr<CatPack>(new JsonCatPack(manifest)));
            } catch (const Exception& e) {
                themeWarningLog() << "Couldn't load catpack json:" << e.cause();
            }
        } else {
            loadFiles(dir);
        }
    }
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
}

void ThemeManager::refresh()
{
    m_themes.clear();
    m_icons.clear();
<<<<<<< HEAD

    initializeThemes();
=======
    m_catPacks.clear();

    initializeThemes();
    initializeCatPacks();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
}
