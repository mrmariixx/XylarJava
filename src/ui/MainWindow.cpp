#include "ui/MainWindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPixmap>
#include <QPair>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include "ui/XylarNavBar.h"

namespace xylar {
namespace {

constexpr int kWindowMinWidth = 1040;
constexpr int kWindowMinHeight = 700;

void addDropShadow(QWidget *widget, const QColor &color, int blur, const QPointF &offset)
{
    auto *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(blur);
    shadow->setOffset(offset);
    shadow->setColor(color);
    widget->setGraphicsEffect(shadow);
}

QLabel *makeIconLabel(const QString &resource, int size)
{
    auto *label = new QLabel;
    label->setFixedSize(size, size);
    label->setPixmap(QPixmap(resource).scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    return label;
}

QString displayVersion(const MinecraftVersion &version)
{
    return version.type.isEmpty() ? version.id : QStringLiteral("%1  (%2)").arg(version.id, version.type);
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1220, 820);
    setMinimumSize(kWindowMinWidth, kWindowMinHeight);
    setWindowTitle(QStringLiteral("XylarJava"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/app-icon.png")));

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("Shell"));
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(32, 26, 32, 22);
    root->setSpacing(16);

    auto *topBar = new QHBoxLayout;
    topBar->setSpacing(14);
    topBar->addWidget(makeIconLabel(QStringLiteral(":/icons/app-icon.png"), 52));

    auto *titleBlock = new QVBoxLayout;
    titleBlock->setSpacing(2);
    auto *title = new QLabel(QStringLiteral("XylarJava"));
    title->setObjectName(QStringLiteral("AppTitle"));
    titleBlock->addWidget(title);
    titleBlock->addWidget(createMutedText(QStringLiteral("Minecraft Java launcher")));
    topBar->addLayout(titleBlock);
    topBar->addStretch(1);

    auto *statusBadge = new QLabel(QStringLiteral("Ready"));
    statusBadge->setObjectName(QStringLiteral("StatusBadge"));
    statusBadge->setAlignment(Qt::AlignCenter);
    topBar->addWidget(statusBadge);
    root->addLayout(topBar);

    m_pages = new QStackedWidget;
    m_pages->addWidget(createHomePage());
    m_pages->addWidget(createModpacksPage());
    m_pages->addWidget(createSettingsPage());
    root->addWidget(m_pages, 1);

    auto *navRow = new QHBoxLayout;
    navRow->addStretch(1);
    m_navBar = new XylarNavBar;
    m_navBar->setFixedWidth(520);
    m_navBar->setItems({
        {QStringLiteral("home"), QStringLiteral("Home"), QIcon(QStringLiteral(":/icons/home.svg"))},
        {QStringLiteral("modpacks"), QStringLiteral("Modpacks"), QIcon(QStringLiteral(":/icons/package.svg"))},
        {QStringLiteral("settings"), QStringLiteral("Settings"), QIcon(QStringLiteral(":/icons/settings.svg"))},
    });
    connect(m_navBar, &XylarNavBar::currentChanged, this, [this](const QString &, int index) {
        m_pages->setCurrentIndex(index);
    });
    navRow->addWidget(m_navBar);
    navRow->addStretch(1);
    root->addLayout(navRow);

    connect(&m_controller, &LauncherController::logLine, this, &MainWindow::appendLog);
    connect(&m_controller, &LauncherController::versionsChanged, this, [this](const QList<MinecraftVersion> &) {
        refreshVersionList();
    });
    connect(&m_controller, &LauncherController::instancesChanged, this, [this](const QList<Instance> &) {
        refreshInstanceList();
    });
    connect(&m_controller, &LauncherController::progressChanged, this, [this](int value, int maximum, const QString &label) {
        m_progress->setMaximum(qMax(1, maximum));
        m_progress->setValue(value);
        if (!label.isEmpty()) {
            m_progress->setFormat(QStringLiteral("%1/%2  %3").arg(value).arg(qMax(1, maximum)).arg(label));
        }
    });

    setStyleSheet(QStringLiteral(R"(
        #Shell {
            background: #030303;
            font-family: "Segoe UI";
        }
        QLabel {
            color: #f5f5f7;
            letter-spacing: 0px;
        }
        #AppTitle {
            font-size: 28px;
            font-weight: 700;
        }
        #SectionTitle {
            font-size: 22px;
            font-weight: 700;
        }
        #HeroTitle {
            color: #f5f5f7;
            font-size: 34px;
            font-weight: 750;
        }
        #MicroLabel {
            color: rgba(245, 245, 247, 118);
            font-size: 11px;
            font-weight: 700;
            text-transform: uppercase;
        }
        #MutedText {
            color: rgba(245, 245, 247, 145);
            font-size: 13px;
        }
        #StatusBadge {
            min-width: 104px;
            min-height: 34px;
            padding: 0 16px;
            border-radius: 17px;
            color: #f5f5f7;
            background: rgba(255, 255, 255, 22);
            border: 1px solid rgba(255, 255, 255, 34);
        }
        #Panel, #HeroPanel {
            border-radius: 24px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(255,255,255,22),
                stop:0.48 rgba(255,255,255,13),
                stop:1 rgba(255,255,255,8));
            border: 1px solid rgba(255, 255, 255, 32);
        }
        #InsetPanel {
            border-radius: 18px;
            background: rgba(255, 255, 255, 12);
            border: 1px solid rgba(255, 255, 255, 22);
        }
        #MetricPill {
            min-height: 58px;
            border-radius: 18px;
            background: rgba(255, 255, 255, 12);
            border: 1px solid rgba(255, 255, 255, 24);
        }
        QPushButton {
            min-height: 42px;
            padding: 0 18px;
            border-radius: 21px;
            color: #050505;
            font-weight: 700;
            background: #f5f5f7;
            border: 1px solid rgba(255,255,255,130);
        }
        QPushButton#SecondaryButton {
            color: #f5f5f7;
            background: rgba(255, 255, 255, 14);
            border: 1px solid rgba(255,255,255,30);
        }
        QPushButton#SecondaryButton:hover {
            background: rgba(255, 255, 255, 24);
        }
        QPushButton#PrimaryButton {
            color: #050505;
            background: #f5f5f7;
        }
        QPushButton:hover {
            background: #ffffff;
        }
        QLineEdit, QListWidget, QComboBox, QSpinBox, QPlainTextEdit {
            color: #f5f5f7;
            background: rgba(255, 255, 255, 10);
            border: 1px solid rgba(255, 255, 255, 24);
            border-radius: 12px;
            padding: 9px;
            selection-background-color: rgba(255, 255, 255, 72);
        }
        QComboBox, QLineEdit, QSpinBox {
            min-height: 28px;
        }
        QPlainTextEdit {
            font-family: "Cascadia Mono", "Consolas";
            font-size: 12px;
        }
        QListWidget::item {
            min-height: 42px;
            border-radius: 10px;
            padding: 6px 10px;
        }
        QListWidget::item:selected, QListWidget::item:hover {
            background: rgba(255, 255, 255, 42);
            color: #f5f5f7;
        }
        QProgressBar {
            min-height: 16px;
            border-radius: 8px;
            background: rgba(255, 255, 255, 14);
            border: 0px;
            color: #f5f5f7;
            text-align: center;
        }
        QProgressBar::chunk {
            border-radius: 8px;
            background: #f5f5f7;
        }
        QCheckBox {
            color: #f5f5f7;
            spacing: 8px;
        }
    )"));

    refreshInstanceList();
    appendLog(QStringLiteral("XylarJava ready."));
    appendLog(QStringLiteral("Java: %1").arg(m_controller.javaPath().isEmpty() ? QStringLiteral("not found") : m_controller.javaPath()));
    QTimer::singleShot(250, this, [this]() {
        m_controller.refreshVersions();
    });
}

QWidget *MainWindow::createHomePage()
{
    auto *page = new QWidget;
    auto *layout = new QGridLayout(page);
    layout->setContentsMargins(0, 4, 0, 0);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(18);

    auto *hero = createPanel(QStringLiteral("HeroPanel"));
    addDropShadow(hero, QColor(0, 0, 0, 92), 42, QPointF(0, 20));
    auto *heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(30, 28, 30, 28);
    heroLayout->setSpacing(18);

    auto *heroTop = new QHBoxLayout;
    heroTop->setSpacing(18);
    heroTop->addWidget(makeIconLabel(QStringLiteral(":/icons/app-icon.png"), 88));

    auto *headlineBlock = new QVBoxLayout;
    headlineBlock->setSpacing(4);
    auto *heroTitle = new QLabel(QStringLiteral("Ready to launch."));
    heroTitle->setObjectName(QStringLiteral("HeroTitle"));
    headlineBlock->addWidget(heroTitle);
    headlineBlock->addWidget(createMutedText(QStringLiteral("Choose a Minecraft version, attach a loader, install an instance, then launch.")));
    heroTop->addLayout(headlineBlock, 1);
    heroLayout->addLayout(heroTop);

    auto *metrics = new QHBoxLayout;
    metrics->setSpacing(12);
    const QList<QPair<QString, QString>> metricItems = {
        {QStringLiteral("Versions"), QStringLiteral("Mojang index")},
        {QStringLiteral("Loaders"), QStringLiteral("Vanilla / Fabric / Forge")},
        {QStringLiteral("Mode"), QStringLiteral("Offline launch")}
    };
    for (const auto &item : metricItems) {
        auto *metric = new QFrame;
        metric->setObjectName(QStringLiteral("MetricPill"));
        auto *metricLayout = new QVBoxLayout(metric);
        metricLayout->setContentsMargins(16, 10, 16, 10);
        metricLayout->setSpacing(2);
        auto *micro = new QLabel(item.first);
        micro->setObjectName(QStringLiteral("MicroLabel"));
        auto *value = new QLabel(item.second);
        value->setObjectName(QStringLiteral("MutedText"));
        metricLayout->addWidget(micro);
        metricLayout->addWidget(value);
        metrics->addWidget(metric);
    }
    heroLayout->addLayout(metrics);

    auto *builder = new QFrame;
    builder->setObjectName(QStringLiteral("InsetPanel"));
    auto *builderLayout = new QGridLayout(builder);
    builderLayout->setContentsMargins(18, 18, 18, 18);
    builderLayout->setHorizontalSpacing(12);
    builderLayout->setVerticalSpacing(12);

    auto *versionLabel = new QLabel(QStringLiteral("Version"));
    versionLabel->setObjectName(QStringLiteral("MicroLabel"));
    m_versionFilterCombo = new QComboBox;
    m_versionFilterCombo->addItem(QStringLiteral("Release"), QStringLiteral("release"));
    m_versionFilterCombo->addItem(QStringLiteral("Snapshot"), QStringLiteral("snapshot"));
    m_versionFilterCombo->addItem(QStringLiteral("Old beta"), QStringLiteral("old_beta"));
    m_versionFilterCombo->addItem(QStringLiteral("Old alpha"), QStringLiteral("old_alpha"));
    m_versionFilterCombo->addItem(QStringLiteral("All"), QStringLiteral("all"));
    m_versionCombo = new QComboBox;
    m_versionCombo->addItem(QStringLiteral("Loading versions..."), QString());

    auto *loaderLabel = new QLabel(QStringLiteral("Loader"));
    loaderLabel->setObjectName(QStringLiteral("MicroLabel"));
    m_loaderCombo = new QComboBox;
    m_loaderCombo->addItems({QStringLiteral("Vanilla"), QStringLiteral("Fabric"), QStringLiteral("Forge")});

    auto *instanceLabel = new QLabel(QStringLiteral("Instance"));
    instanceLabel->setObjectName(QStringLiteral("MicroLabel"));
    m_instanceNameEdit = new QLineEdit;
    m_instanceNameEdit->setPlaceholderText(QStringLiteral("Instance name"));
    m_instanceNameEdit->setText(QStringLiteral("Vanilla latest"));

    auto *playerLabel = new QLabel(QStringLiteral("Player"));
    playerLabel->setObjectName(QStringLiteral("MicroLabel"));
    m_playerNameEdit = new QLineEdit;
    m_playerNameEdit->setPlaceholderText(QStringLiteral("Offline player name"));
    m_playerNameEdit->setText(QStringLiteral("XylarPlayer"));

    builderLayout->addWidget(versionLabel, 0, 0);
    builderLayout->addWidget(m_versionFilterCombo, 1, 0);
    builderLayout->addWidget(m_versionCombo, 1, 1, 1, 2);
    builderLayout->addWidget(loaderLabel, 0, 3);
    builderLayout->addWidget(m_loaderCombo, 1, 3);
    builderLayout->addWidget(instanceLabel, 2, 0);
    builderLayout->addWidget(m_instanceNameEdit, 3, 0, 1, 2);
    builderLayout->addWidget(playerLabel, 2, 2);
    builderLayout->addWidget(m_playerNameEdit, 3, 2, 1, 2);
    heroLayout->addWidget(builder);

    auto *actions = new QHBoxLayout;
    actions->setSpacing(10);
    auto *refresh = createActionButton(QStringLiteral("Refresh"), QStringLiteral(":/icons/refresh-cw.svg"));
    auto *install = createActionButton(QStringLiteral("Install"), QStringLiteral(":/icons/download.svg"));
    auto *launch = createActionButton(QStringLiteral("Launch"), QStringLiteral(":/icons/play.svg"));
    refresh->setObjectName(QStringLiteral("SecondaryButton"));
    install->setObjectName(QStringLiteral("SecondaryButton"));
    launch->setObjectName(QStringLiteral("PrimaryButton"));
    actions->addWidget(refresh);
    actions->addWidget(install);
    actions->addWidget(launch);
    actions->addStretch(1);
    heroLayout->addLayout(actions);

    m_progress = new QProgressBar;
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_progress->setFormat(QStringLiteral("Idle"));
    heroLayout->addWidget(m_progress);

    connect(refresh, &QPushButton::clicked, this, [this]() {
        m_controller.refreshVersions();
    });
    connect(install, &QPushButton::clicked, this, [this]() {
        const QString versionId = selectedVersionId();
        if (versionId.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("XylarJava"), QStringLiteral("Refresh versions first."));
            return;
        }
        const QString instanceName = m_instanceNameEdit->text().trimmed().isEmpty()
            ? QStringLiteral("Minecraft %1").arg(versionId)
            : m_instanceNameEdit->text().trimmed();
        m_controller.installInstance(versionId, selectedLoader(), instanceName);
    });
    connect(m_versionFilterCombo, &QComboBox::currentIndexChanged, this, [this]() {
        refreshVersionList();
    });
    connect(launch, &QPushButton::clicked, this, [this]() {
        const QString id = selectedInstanceId();
        if (id.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("XylarJava"), QStringLiteral("Install or select an instance first."));
            return;
        }
        m_controller.launchInstance(id, m_playerNameEdit->text(), m_minMemorySpin->value(), m_maxMemorySpin->value(), effectiveJavaPath());
    });

    auto *logPanel = createPanel(QStringLiteral("Panel"));
    auto *logLayout = new QVBoxLayout(logPanel);
    logLayout->setContentsMargins(20, 18, 20, 18);
    logLayout->setSpacing(10);
    logLayout->addWidget(createSectionTitle(QStringLiteral("Activity")));
    m_log = new QPlainTextEdit;
    m_log->setReadOnly(true);
    m_log->setMinimumHeight(160);
    logLayout->addWidget(m_log, 1);

    auto *side = createPanel(QStringLiteral("Panel"));
    auto *sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(22, 22, 22, 22);
    sideLayout->setSpacing(14);
    sideLayout->addWidget(createSectionTitle(QStringLiteral("Instances")));
    sideLayout->addWidget(createMutedText(QStringLiteral("Installed profiles are stored locally and can be launched offline.")));
    m_instanceList = new QListWidget;
    sideLayout->addWidget(m_instanceList, 1);

    auto *quickPanel = createPanel(QStringLiteral("Panel"));
    auto *quickLayout = new QVBoxLayout(quickPanel);
    quickLayout->setContentsMargins(22, 20, 22, 20);
    quickLayout->setSpacing(10);
    quickLayout->addWidget(createSectionTitle(QStringLiteral("Runtime")));
    quickLayout->addWidget(createMutedText(QStringLiteral("Java 21 is downloaded automatically when a launch or Forge setup needs it.")));
    quickLayout->addStretch(1);

    layout->addWidget(hero, 0, 0, 2, 2);
    layout->addWidget(side, 0, 2, 2, 1);
    layout->addWidget(logPanel, 2, 0, 1, 2);
    layout->addWidget(quickPanel, 2, 2, 1, 1);
    layout->setColumnStretch(0, 2);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 2);
    layout->setRowStretch(0, 2);
    layout->setRowStretch(1, 2);
    layout->setRowStretch(2, 1);

    return page;
}

QWidget *MainWindow::createModpacksPage()
{
    auto *page = new QWidget;
    auto *layout = new QGridLayout(page);
    layout->setContentsMargins(0, 4, 0, 0);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(18);

    auto *panel = createPanel(QStringLiteral("Panel"));
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(26, 24, 26, 24);
    panelLayout->setSpacing(14);
    panelLayout->addWidget(createSectionTitle(QStringLiteral("Modpacks")));
    panelLayout->addWidget(createMutedText(QStringLiteral("Import Modrinth packs, add local jar mods, and open the selected instance folder.")));

    auto *dropZone = new QFrame;
    dropZone->setObjectName(QStringLiteral("InsetPanel"));
    auto *dropLayout = new QVBoxLayout(dropZone);
    dropLayout->setContentsMargins(22, 22, 22, 22);
    dropLayout->setSpacing(8);
    auto *dropTitle = new QLabel(QStringLiteral("Modrinth pack import"));
    dropTitle->setObjectName(QStringLiteral("SectionTitle"));
    dropLayout->addWidget(dropTitle);
    dropLayout->addWidget(createMutedText(QStringLiteral("XylarJava reads modrinth.index.json, resolves Minecraft and loader dependencies, downloads declared files, and copies overrides.")));
    panelLayout->addWidget(dropZone);

    auto *importPack = createActionButton(QStringLiteral("Import .mrpack"), QStringLiteral(":/icons/import.svg"));
    auto *openMods = createActionButton(QStringLiteral("Open mods folder"), QStringLiteral(":/icons/folder-open.svg"));
    auto *importMods = createActionButton(QStringLiteral("Import jar mods"), QStringLiteral(":/icons/download.svg"));
    importPack->setObjectName(QStringLiteral("PrimaryButton"));

    auto *buttons = new QHBoxLayout;
    buttons->setSpacing(10);
    buttons->addWidget(importPack);
    buttons->addWidget(openMods);
    buttons->addWidget(importMods);
    buttons->addStretch(1);
    panelLayout->addLayout(buttons);
    panelLayout->addStretch(1);

    connect(importPack, &QPushButton::clicked, this, [this]() {
        const QString file = QFileDialog::getOpenFileName(this, QStringLiteral("Import Modrinth pack"), QString(), QStringLiteral("Modrinth packs (*.mrpack);;All files (*.*)"));
        if (!file.isEmpty()) {
            m_controller.importModrinthPack(file);
        }
    });

    connect(openMods, &QPushButton::clicked, this, [this]() {
        const QString id = selectedInstanceId();
        if (id.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("XylarJava"), QStringLiteral("Select an installed instance first."));
            return;
        }
        for (const Instance &item : m_controller.instances()) {
            if (item.id == id) {
                QDir().mkpath(QDir(item.gamePath).filePath(QStringLiteral("mods")));
                QDesktopServices::openUrl(QUrl::fromLocalFile(QDir(item.gamePath).filePath(QStringLiteral("mods"))));
                return;
            }
        }
    });

    connect(importMods, &QPushButton::clicked, this, [this]() {
        const QString id = selectedInstanceId();
        if (id.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("XylarJava"), QStringLiteral("Select an installed instance first."));
            return;
        }

        Instance target;
        for (const Instance &item : m_controller.instances()) {
            if (item.id == id) {
                target = item;
                break;
            }
        }
        if (!target.isValid()) {
            return;
        }

        const QStringList files = QFileDialog::getOpenFileNames(this, QStringLiteral("Import mods"), QString(), QStringLiteral("Mod jars (*.jar)"));
        const QString modsDir = QDir(target.gamePath).filePath(QStringLiteral("mods"));
        QDir().mkpath(modsDir);
        for (const QString &file : files) {
            QFile::copy(file, QDir(modsDir).filePath(QFileInfo(file).fileName()));
        }
        appendLog(QStringLiteral("Imported %1 mod file(s) into %2.").arg(files.size()).arg(target.name));
    });

    auto *sources = createPanel(QStringLiteral("Panel"));
    auto *sourcesLayout = new QVBoxLayout(sources);
    sourcesLayout->setContentsMargins(24, 22, 24, 22);
    sourcesLayout->setSpacing(12);
    sourcesLayout->addWidget(createSectionTitle(QStringLiteral("Sources")));
    const QList<QPair<QString, QString>> rows = {
        {QStringLiteral("Modrinth"), QStringLiteral(".mrpack with hashes and overrides")},
        {QStringLiteral("Local mods"), QStringLiteral(".jar files copied into mods")},
        {QStringLiteral("Instances"), QStringLiteral("Use the selected profile")}
    };
    for (const auto &row : rows) {
        auto *item = new QFrame;
        item->setObjectName(QStringLiteral("InsetPanel"));
        auto *itemLayout = new QVBoxLayout(item);
        itemLayout->setContentsMargins(16, 12, 16, 12);
        itemLayout->setSpacing(2);
        auto *name = new QLabel(row.first);
        name->setObjectName(QStringLiteral("MicroLabel"));
        itemLayout->addWidget(name);
        itemLayout->addWidget(createMutedText(row.second));
        sourcesLayout->addWidget(item);
    }
    sourcesLayout->addStretch(1);

    layout->addWidget(panel, 0, 0, 1, 2);
    layout->addWidget(sources, 0, 2);
    layout->setColumnStretch(0, 2);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
    return page;
}

QWidget *MainWindow::createSettingsPage()
{
    auto *page = new QWidget;
    auto *layout = new QGridLayout(page);
    layout->setContentsMargins(0, 4, 0, 0);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(18);

    auto *runtime = createPanel(QStringLiteral("Panel"));
    auto *runtimeLayout = new QVBoxLayout(runtime);
    runtimeLayout->setContentsMargins(26, 24, 26, 24);
    runtimeLayout->setSpacing(14);
    runtimeLayout->addWidget(createSectionTitle(QStringLiteral("Runtime")));
    runtimeLayout->addWidget(createMutedText(QStringLiteral("Java path and memory used for launch plans. Oracle JDK 21 can be installed automatically.")));

    auto *javaBox = new QFrame;
    javaBox->setObjectName(QStringLiteral("InsetPanel"));
    auto *javaBoxLayout = new QVBoxLayout(javaBox);
    javaBoxLayout->setContentsMargins(18, 16, 18, 18);
    javaBoxLayout->setSpacing(10);
    auto *javaLabel = new QLabel(QStringLiteral("Java executable"));
    javaLabel->setObjectName(QStringLiteral("MicroLabel"));
    javaBoxLayout->addWidget(javaLabel);
    auto *javaRow = new QHBoxLayout;
    m_javaPathEdit = new QLineEdit;
    m_javaPathEdit->setPlaceholderText(QStringLiteral("Auto detect Java"));
    m_javaPathEdit->setText(m_controller.javaPath());
    auto *browseJava = createActionButton(QStringLiteral("Browse"), QStringLiteral(":/icons/folder-open.svg"));
    auto *installJava = createActionButton(QStringLiteral("Install Oracle JDK 21"), QStringLiteral(":/icons/download.svg"));
    javaRow->addWidget(m_javaPathEdit, 1);
    javaRow->addWidget(browseJava);
    javaBoxLayout->addLayout(javaRow);
    javaBoxLayout->addWidget(installJava);
    runtimeLayout->addWidget(javaBox);

    auto *memoryBox = new QFrame;
    memoryBox->setObjectName(QStringLiteral("InsetPanel"));
    auto *memoryBoxLayout = new QVBoxLayout(memoryBox);
    memoryBoxLayout->setContentsMargins(18, 16, 18, 18);
    memoryBoxLayout->setSpacing(10);
    auto *memoryLabel = new QLabel(QStringLiteral("Memory"));
    memoryLabel->setObjectName(QStringLiteral("MicroLabel"));
    memoryBoxLayout->addWidget(memoryLabel);
    auto *memoryRow = new QHBoxLayout;
    m_minMemorySpin = new QSpinBox;
    m_minMemorySpin->setRange(256, 32768);
    m_minMemorySpin->setSingleStep(256);
    m_minMemorySpin->setValue(1024);
    m_minMemorySpin->setSuffix(QStringLiteral(" MB min"));
    m_maxMemorySpin = new QSpinBox;
    m_maxMemorySpin->setRange(512, 65536);
    m_maxMemorySpin->setSingleStep(512);
    m_maxMemorySpin->setValue(4096);
    m_maxMemorySpin->setSuffix(QStringLiteral(" MB max"));
    memoryRow->addWidget(m_minMemorySpin);
    memoryRow->addWidget(m_maxMemorySpin);
    memoryBoxLayout->addLayout(memoryRow);
    runtimeLayout->addWidget(memoryBox);

    connect(browseJava, &QPushButton::clicked, this, [this]() {
        const QString file = QFileDialog::getOpenFileName(this, QStringLiteral("Select Java executable"), QString(), QStringLiteral("Java executable (java.exe javaw.exe);;All files (*.*)"));
        if (!file.isEmpty()) {
            m_javaPathEdit->setText(file);
        }
    });
    connect(installJava, &QPushButton::clicked, this, [this]() {
        const QString java = m_controller.ensureJavaRuntime();
        if (!java.isEmpty()) {
            m_javaPathEdit->setText(java);
        }
    });

    auto *account = createPanel(QStringLiteral("Panel"));
    auto *accountLayout = new QVBoxLayout(account);
    accountLayout->setContentsMargins(26, 24, 26, 24);
    accountLayout->setSpacing(12);
    accountLayout->addWidget(createSectionTitle(QStringLiteral("Account")));
    accountLayout->addWidget(createMutedText(m_controller.accountSummary()));
    auto *offline = new QCheckBox(QStringLiteral("Offline mode"));
    offline->setChecked(true);
    offline->setEnabled(false);
    accountLayout->addWidget(offline);
    accountLayout->addStretch(1);

    auto *data = createPanel(QStringLiteral("Panel"));
    auto *dataLayout = new QVBoxLayout(data);
    dataLayout->setContentsMargins(26, 24, 26, 24);
    dataLayout->setSpacing(12);
    dataLayout->addWidget(createSectionTitle(QStringLiteral("Data")));
    dataLayout->addWidget(createMutedText(QStringLiteral("Instances, libraries, assets and Forge profiles are stored beside the executable in data/.")));
    dataLayout->addStretch(1);

    layout->addWidget(runtime, 0, 0, 2, 2);
    layout->addWidget(account, 0, 2);
    layout->addWidget(data, 1, 2);
    layout->setColumnStretch(0, 2);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 2);
    return page;
}

QFrame *MainWindow::createPanel(const QString &objectName)
{
    auto *panel = new QFrame;
    panel->setObjectName(objectName);
    panel->setFrameShape(QFrame::NoFrame);
    return panel;
}

QLabel *MainWindow::createSectionTitle(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName(QStringLiteral("SectionTitle"));
    label->setWordWrap(true);
    return label;
}

QLabel *MainWindow::createMutedText(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName(QStringLiteral("MutedText"));
    label->setWordWrap(true);
    return label;
}

QPushButton *MainWindow::createActionButton(const QString &text, const QString &iconResource)
{
    auto *button = new QPushButton(QIcon(iconResource), text);
    button->setObjectName(QStringLiteral("SecondaryButton"));
    button->setIconSize(QSize(18, 18));
    return button;
}

void MainWindow::refreshVersionList()
{
    m_versionCombo->clear();
    const QList<MinecraftVersion> versions = m_controller.cachedVersions();
    const QString filter = m_versionFilterCombo ? m_versionFilterCombo->currentData().toString() : QStringLiteral("release");
    for (const MinecraftVersion &version : versions) {
        if (filter == QStringLiteral("all") || version.type == filter) {
            m_versionCombo->addItem(displayVersion(version), version.id);
        }
    }
    if (m_versionCombo->count() == 0) {
        m_versionCombo->addItem(QStringLiteral("No versions loaded for this filter"), QString());
    }
}

void MainWindow::refreshInstanceList()
{
    if (!m_instanceList) {
        return;
    }

    m_instanceList->clear();
    const QList<Instance> items = m_controller.instances();
    for (const Instance &instance : items) {
        auto *item = new QListWidgetItem(QStringLiteral("%1  -  %2  -  %3").arg(instance.name, instance.loader, instance.minecraftVersion.id));
        item->setData(Qt::UserRole, instance.id);
        m_instanceList->addItem(item);
    }
    if (m_instanceList->count() > 0) {
        m_instanceList->setCurrentRow(0);
    }
}

void MainWindow::appendLog(const QString &line)
{
    if (!m_log) {
        return;
    }
    m_log->appendPlainText(QStringLiteral("[%1] %2").arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")), line));
}

QString MainWindow::selectedVersionId() const
{
    return m_versionCombo ? m_versionCombo->currentData().toString() : QString();
}

QString MainWindow::selectedInstanceId() const
{
    if (!m_instanceList || !m_instanceList->currentItem()) {
        return {};
    }
    return m_instanceList->currentItem()->data(Qt::UserRole).toString();
}

QString MainWindow::selectedLoader() const
{
    return m_loaderCombo ? m_loaderCombo->currentText() : QStringLiteral("Vanilla");
}

QString MainWindow::effectiveJavaPath() const
{
    return m_javaPathEdit ? m_javaPathEdit->text().trimmed() : QString();
}

} // namespace xylar
