#include "ui/MainWindow.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPixmap>
#include <QPair>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace xylar {
namespace {

constexpr int kWindowMinWidth = 1040;
constexpr int kWindowMinHeight = 700;

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

class AppShell final : public QWidget
{
public:
    using QWidget::QWidget;

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), QColor(18, 18, 20));
    }
};

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1220, 820);
    setMinimumSize(kWindowMinWidth, kWindowMinHeight);
    setWindowTitle(QStringLiteral("Launcher"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/app-icon.png")));

    auto *central = new AppShell(this);
    central->setObjectName(QStringLiteral("Shell"));
    setCentralWidget(central);

    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_pages = new QStackedWidget;
    m_pages->setObjectName(QStringLiteral("Pages"));
    m_pages->addWidget(createHomePage());
    m_pages->addWidget(createModpacksPage());
    m_pages->addWidget(createSettingsPage());

    auto *sideBar = new QFrame;
    sideBar->setObjectName(QStringLiteral("SideBar"));
    sideBar->setFixedWidth(236);
    auto *sideLayout = new QVBoxLayout(sideBar);
    sideLayout->setContentsMargins(18, 20, 18, 18);
    sideLayout->setSpacing(10);

    auto *brandRow = new QFrame;
    brandRow->setObjectName(QStringLiteral("BrandRow"));
    auto *brandLayout = new QHBoxLayout(brandRow);
    brandLayout->setContentsMargins(0, 0, 0, 0);
    brandLayout->setSpacing(12);
    brandLayout->addWidget(makeIconLabel(QStringLiteral(":/icons/app-icon.png"), 38));
    auto *brandName = new QLabel(QStringLiteral("XylarJava"));
    brandName->setObjectName(QStringLiteral("BrandName"));
    brandLayout->addWidget(brandName, 1);
    sideLayout->addWidget(brandRow);
    sideLayout->addSpacing(18);

    m_instanceSearchEdit = new QLineEdit;
    m_instanceSearchEdit->setObjectName(QStringLiteral("SidebarSearch"));
    m_instanceSearchEdit->setPlaceholderText(QStringLiteral("Search"));
    sideLayout->addWidget(m_instanceSearchEdit);
    sideLayout->addSpacing(12);

    auto *navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    auto makeNavButton = [navGroup](const QString &icon, const QString &label, int index) {
        auto *button = new QPushButton(QIcon(icon), label);
        button->setObjectName(QStringLiteral("SideNavButton"));
        button->setCheckable(true);
        button->setToolTip(label);
        button->setFixedHeight(44);
        button->setIconSize(QSize(21, 21));
        button->setCursor(Qt::PointingHandCursor);
        navGroup->addButton(button, index);
        return button;
    };

    auto *homeNav = makeNavButton(QStringLiteral(":/icons/library.svg"), QStringLiteral("Library"), 0);
    auto *modpacksNav = makeNavButton(QStringLiteral(":/icons/modpacks.svg"), QStringLiteral("Modpacks"), 1);
    auto *settingsNav = makeNavButton(QStringLiteral(":/icons/preferences.svg"), QStringLiteral("Settings"), 2);
    homeNav->setChecked(true);
    sideLayout->addWidget(homeNav);
    sideLayout->addWidget(modpacksNav);
    sideLayout->addWidget(settingsNav);
    sideLayout->addStretch(1);

    auto *accountChip = new QFrame;
    accountChip->setObjectName(QStringLiteral("AccountChip"));
    auto *accountLayout = new QHBoxLayout(accountChip);
    accountLayout->setContentsMargins(10, 8, 10, 8);
    accountLayout->setSpacing(8);
    accountLayout->addWidget(makeIconLabel(QStringLiteral(":/icons/user.svg"), 20));
    auto *accountLabel = new QLabel(QStringLiteral("Offline"));
    accountLabel->setObjectName(QStringLiteral("AccountLabel"));
    accountLayout->addWidget(accountLabel, 1);
    sideLayout->addWidget(accountChip);

    connect(navGroup, &QButtonGroup::idClicked, this, [this](int index) {
        m_pages->setCurrentIndex(index);
    });
    connect(m_instanceSearchEdit, &QLineEdit::textChanged, this, [this]() {
        refreshInstanceList();
    });

    auto *content = new QWidget;
    content->setObjectName(QStringLiteral("Content"));
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(26, 24, 28, 22);
    contentLayout->setSpacing(14);

    auto *topBar = new QHBoxLayout;
    topBar->setContentsMargins(0, 0, 0, 0);
    topBar->addStretch(1);
    auto *statusBadge = new QLabel(QStringLiteral("Ready"));
    statusBadge->setObjectName(QStringLiteral("StatusBadge"));
    statusBadge->setAlignment(Qt::AlignCenter);
    topBar->addWidget(statusBadge);
    contentLayout->addLayout(topBar);
    contentLayout->addWidget(m_pages, 1);

    root->addWidget(sideBar);
    root->addWidget(content, 1);

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
            background: transparent;
            font-family: "Segoe UI";
        }
        #SideBar {
            background: #0f0f10;
            border-right: 1px solid #242427;
        }
        #Content {
            background: transparent;
        }
        #Pages {
            background: transparent;
        }
        QLabel {
            color: #f5f5f7;
            letter-spacing: 0px;
        }
        #BrandName {
            color: #f5f5f7;
            font-size: 17px;
            font-weight: 750;
        }
        #AccountChip {
            border-radius: 8px;
            background: #18181b;
            border: 1px solid #2a2a2f;
        }
        #AccountLabel {
            color: #d8d8dc;
            font-size: 12px;
            font-weight: 700;
        }
        #SidebarSearch {
            min-height: 32px;
            padding: 0 12px;
            border-radius: 8px;
            color: #eeeeef;
            background: #202024;
            border: 1px solid #303036;
        }
        #SidebarSearch:focus {
            background: #242429;
            border: 1px solid #4a4a52;
        }
        #PageTitle {
            color: #f5f5f7;
            font-size: 32px;
            font-weight: 750;
        }
        #SectionTitle {
            font-size: 20px;
            font-weight: 700;
        }
        #DropTitle {
            color: #f5f5f7;
            font-size: 28px;
            font-weight: 750;
        }
        #MicroLabel {
            color: rgba(245, 245, 247, 132);
            font-size: 11px;
            font-weight: 700;
            text-transform: uppercase;
        }
        #MutedText {
            color: rgba(245, 245, 247, 154);
            font-size: 13px;
        }
        #StatusBadge {
            min-width: 104px;
            min-height: 34px;
            padding: 0 16px;
            border-radius: 6px;
            color: #cfcfd4;
            background: #1a1a1d;
            border: 1px solid #2a2a2f;
        }
        #Panel, #HeroPanel {
            border-radius: 8px;
            background: #18181b;
            border: 1px solid #2a2a2f;
        }
        #InsetPanel {
            border-radius: 6px;
            background: #202024;
            border: 1px solid #303036;
        }
        #MetricPill {
            min-height: 58px;
            border-radius: 6px;
            background: #202024;
            border: 1px solid #303036;
        }
        #MetricPill:hover, #InsetPanel:hover {
            background: #242429;
            border: 1px solid #3a3a41;
        }
        #InstanceRow {
            border-radius: 6px;
            background: #202024;
            border: 1px solid #303036;
        }
        #GameTile {
            border-radius: 8px;
            background: #202024;
            border: 1px solid #303036;
        }
        #GameTile:hover {
            background: #25252a;
            border: 1px solid #41414a;
        }
        #CoverTile {
            border-radius: 8px;
            background: #2a2a30;
            border: 1px solid #3a3a43;
        }
        #InstanceTitle {
            color: #f5f5f7;
            font-size: 14px;
            font-weight: 700;
        }
        #InstanceMeta {
            color: rgba(245, 245, 247, 136);
            font-size: 12px;
        }
        #TinyIconBadge {
            border-radius: 8px;
            background: #242429;
            border: 1px solid #383840;
        }
        QPushButton {
            min-height: 42px;
            padding: 0 20px;
            border-radius: 6px;
            color: #050505;
            font-weight: 700;
            background: #f5f5f7;
            border: 1px solid rgba(255,255,255,130);
        }
        QPushButton#SideNavButton {
            min-height: 0px;
            padding: 0 14px;
            border-radius: 8px;
            color: #c7c7cc;
            background: transparent;
            border: 1px solid transparent;
            font-size: 13px;
            font-weight: 700;
            text-align: left;
        }
        QPushButton#SideNavButton:hover {
            color: #ffffff;
            background: #1d1d21;
            border: 1px solid #2b2b31;
        }
        QPushButton#SideNavButton:checked {
            color: #ffffff;
            background: #2a2a30;
            border: 1px solid #3a3a43;
        }
        QPushButton#SecondaryButton {
            color: #f5f5f7;
            background: #222226;
            border: 1px solid #36363d;
        }
        QPushButton#SecondaryButton:hover {
            background: #2b2b31;
            border: 1px solid #464650;
        }
        QPushButton#PrimaryButton {
            color: #050505;
            background: #f5f5f7;
        }
        QPushButton:hover {
            background: #ffffff;
        }
        QPushButton:pressed {
            padding-top: 1px;
            background: rgba(226, 226, 230, 255);
        }
        QLineEdit, QListWidget, QComboBox, QSpinBox, QPlainTextEdit {
            color: #f5f5f7;
            background: #202024;
            border: 1px solid #303036;
            border-radius: 12px;
            padding: 9px;
            selection-background-color: rgba(255, 255, 255, 72);
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QPlainTextEdit:focus {
            background: #242429;
            border: 1px solid #575762;
        }
        QComboBox, QLineEdit, QSpinBox {
            min-height: 28px;
        }
        QComboBox::drop-down {
            width: 30px;
            border: 0px;
        }
        QComboBox::down-arrow {
            image: none;
            width: 0px;
            height: 0px;
        }
        QComboBox QAbstractItemView {
            color: #f5f5f7;
            background: #151516;
            border: 1px solid rgba(255,255,255,45);
            selection-background-color: rgba(255,255,255,42);
            outline: 0px;
        }
        QPlainTextEdit {
            font-family: "Cascadia Mono", "Consolas";
            font-size: 12px;
        }
        QListWidget {
            outline: 0px;
        }
        QListWidget::item {
            min-height: 62px;
            border-radius: 14px;
            padding: 4px;
            margin: 4px 0px;
        }
        QListWidget::item:selected, QListWidget::item:hover {
            background: rgba(255, 255, 255, 28);
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
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 9px;
            background: rgba(255,255,255,12);
            border: 1px solid rgba(255,255,255,36);
        }
        QCheckBox::indicator:checked {
            background: #f5f5f7;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 8px 0px 8px 0px;
        }
        QScrollBar::handle:vertical {
            min-height: 36px;
            border-radius: 4px;
            background: rgba(255,255,255,38);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
            border: 0px;
            height: 0px;
        }
        QToolTip {
            color: #f5f5f7;
            background: #18181a;
            border: 1px solid rgba(255,255,255,44);
            border-radius: 8px;
            padding: 8px;
        }
    )"));

    refreshInstanceList();
    appendLog(QStringLiteral("Launcher ready."));
    appendLog(QStringLiteral("Java: %1").arg(m_controller.javaPath().isEmpty() ? QStringLiteral("not found") : m_controller.javaPath()));
    QTimer::singleShot(250, this, [this]() {
        m_controller.refreshVersions();
    });
}

QWidget *MainWindow::createHomePage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(18);

    auto *header = new QHBoxLayout;
    header->setContentsMargins(0, 0, 0, 0);
    auto *titleBlock = new QVBoxLayout;
    titleBlock->setSpacing(4);
    auto *pageTitle = new QLabel(QStringLiteral("Library"));
    pageTitle->setObjectName(QStringLiteral("PageTitle"));
    titleBlock->addWidget(pageTitle);
    titleBlock->addWidget(createMutedText(QStringLiteral("Pick a profile and play. If files are missing, I will download them.")));
    header->addLayout(titleBlock, 1);
    auto *refresh = createActionButton(QStringLiteral("Refresh"), QStringLiteral(":/icons/refresh-cw.svg"));
    refresh->setObjectName(QStringLiteral("SecondaryButton"));
    header->addWidget(refresh, 0, Qt::AlignTop);
    layout->addLayout(header);

    auto *contentGrid = new QGridLayout;
    contentGrid->setHorizontalSpacing(18);
    contentGrid->setVerticalSpacing(18);

    auto *installPanel = createPanel(QStringLiteral("Panel"));
    auto *installLayout = new QVBoxLayout(installPanel);
    installLayout->setContentsMargins(22, 20, 22, 20);
    installLayout->setSpacing(14);
    installLayout->addWidget(createSectionTitle(QStringLiteral("Install a profile")));
    installLayout->addWidget(createMutedText(QStringLiteral("Choose a version, name it, then install. Simple.")));

    auto *builder = new QFrame;
    builder->setObjectName(QStringLiteral("InsetPanel"));
    auto *builderLayout = new QGridLayout(builder);
    builderLayout->setContentsMargins(16, 16, 16, 16);
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
    m_playerNameEdit->setText(QStringLiteral("Player"));

    builderLayout->addWidget(versionLabel, 0, 0);
    builderLayout->addWidget(m_versionFilterCombo, 1, 0);
    builderLayout->addWidget(m_versionCombo, 1, 1, 1, 2);
    builderLayout->addWidget(loaderLabel, 0, 3);
    builderLayout->addWidget(m_loaderCombo, 1, 3);
    builderLayout->addWidget(instanceLabel, 2, 0);
    builderLayout->addWidget(m_instanceNameEdit, 3, 0, 1, 2);
    builderLayout->addWidget(playerLabel, 2, 2);
    builderLayout->addWidget(m_playerNameEdit, 3, 2, 1, 2);
    installLayout->addWidget(builder);

    auto *actions = new QHBoxLayout;
    actions->setSpacing(10);
    auto *install = createActionButton(QStringLiteral("Install Instance"), QStringLiteral(":/icons/download.svg"));
    install->setObjectName(QStringLiteral("SecondaryButton"));
    actions->addWidget(install);
    actions->addStretch(1);
    installLayout->addLayout(actions);

    m_progress = new QProgressBar;
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_progress->setFormat(QStringLiteral("Idle"));
    installLayout->addWidget(m_progress);
    installLayout->addStretch(1);

    auto *libraryPanel = createPanel(QStringLiteral("Panel"));
    auto *libraryLayout = new QVBoxLayout(libraryPanel);
    libraryLayout->setContentsMargins(22, 20, 22, 20);
    libraryLayout->setSpacing(14);
    libraryLayout->addWidget(createSectionTitle(QStringLiteral("My games")));

    auto *gameTile = new QFrame;
    gameTile->setObjectName(QStringLiteral("GameTile"));
    auto *gameLayout = new QHBoxLayout(gameTile);
    gameLayout->setContentsMargins(14, 14, 14, 14);
    gameLayout->setSpacing(14);

    auto *cover = new QFrame;
    cover->setObjectName(QStringLiteral("CoverTile"));
    cover->setFixedSize(76, 76);
    auto *coverLayout = new QVBoxLayout(cover);
    coverLayout->setContentsMargins(12, 12, 12, 12);
    coverLayout->addWidget(makeIconLabel(QStringLiteral(":/icons/app-icon.png"), 52), 0, Qt::AlignCenter);
    gameLayout->addWidget(cover);

    auto *gameCopy = new QVBoxLayout;
    gameCopy->setSpacing(3);
    auto *gameTitle = new QLabel(QStringLiteral("Minecraft Java"));
    gameTitle->setObjectName(QStringLiteral("InstanceTitle"));
    auto *gameMeta = new QLabel(QStringLiteral("Local profiles and modded instances"));
    gameMeta->setObjectName(QStringLiteral("InstanceMeta"));
    gameCopy->addWidget(gameTitle);
    gameCopy->addWidget(gameMeta);
    gameLayout->addLayout(gameCopy, 1);

    auto *launch = createActionButton(QStringLiteral("Launch"), QStringLiteral(":/icons/play.svg"));
    launch->setObjectName(QStringLiteral("PrimaryButton"));
    gameLayout->addWidget(launch);
    libraryLayout->addWidget(gameTile);

    m_instanceList = new QListWidget;
    m_instanceList->setSpacing(6);
    libraryLayout->addWidget(m_instanceList, 1);

    connect(refresh, &QPushButton::clicked, this, [this]() {
        m_controller.refreshVersions();
    });
    connect(install, &QPushButton::clicked, this, [this]() {
        const QString versionId = selectedVersionId();
        if (versionId.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Launcher"), QStringLiteral("Refresh versions first."));
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
            QMessageBox::warning(this, QStringLiteral("Launcher"), QStringLiteral("Install or select an instance first."));
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

    contentGrid->addWidget(installPanel, 0, 0);
    contentGrid->addWidget(libraryPanel, 0, 1);
    contentGrid->addWidget(logPanel, 1, 0, 1, 2);
    contentGrid->setColumnStretch(0, 5);
    contentGrid->setColumnStretch(1, 4);
    contentGrid->setRowStretch(0, 3);
    contentGrid->setRowStretch(1, 2);
    layout->addLayout(contentGrid, 1);

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
    panelLayout->addWidget(createMutedText(QStringLiteral("Bring in a .mrpack, add a few jars, or open the mods folder for the selected profile.")));

    auto *dropZone = new QFrame;
    dropZone->setObjectName(QStringLiteral("InsetPanel"));
    auto *dropLayout = new QHBoxLayout(dropZone);
    dropLayout->setContentsMargins(22, 22, 22, 22);
    dropLayout->setSpacing(18);
    auto *dropIcon = new QFrame;
    dropIcon->setObjectName(QStringLiteral("TinyIconBadge"));
    dropIcon->setFixedSize(74, 74);
    auto *dropIconLayout = new QVBoxLayout(dropIcon);
    dropIconLayout->setContentsMargins(15, 15, 15, 15);
    dropIconLayout->addWidget(makeIconLabel(QStringLiteral(":/icons/package.svg"), 44), 0, Qt::AlignCenter);
    dropLayout->addWidget(dropIcon);
    auto *dropCopy = new QVBoxLayout;
    dropCopy->setSpacing(6);
    auto *dropTitle = new QLabel(QStringLiteral("Modrinth pack import"));
    dropTitle->setObjectName(QStringLiteral("DropTitle"));
    dropCopy->addWidget(dropTitle);
    dropCopy->addWidget(createMutedText(QStringLiteral("I read the index, resolve the Minecraft version and loader, download the files, then copy overrides.")));
    dropLayout->addLayout(dropCopy, 1);
    panelLayout->addWidget(dropZone);

    auto *importPack = createActionButton(QStringLiteral("Import .mrpack"), QStringLiteral(":/icons/import.svg"));
    auto *openMods = createActionButton(QStringLiteral("Open mods folder"), QStringLiteral(":/icons/folder-open.svg"));
    auto *importMods = createActionButton(QStringLiteral("Import jar mods"), QStringLiteral(":/icons/download.svg"));
    importPack->setObjectName(QStringLiteral("PrimaryButton"));
    openMods->setObjectName(QStringLiteral("SecondaryButton"));
    importMods->setObjectName(QStringLiteral("SecondaryButton"));

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
            QMessageBox::warning(this, QStringLiteral("Launcher"), QStringLiteral("Select an installed instance first."));
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
            QMessageBox::warning(this, QStringLiteral("Launcher"), QStringLiteral("Select an installed instance first."));
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
    runtimeLayout->addWidget(createMutedText(QStringLiteral("Java and memory settings. If Java 21 is missing, I can install it for you.")));

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
    installJava->setObjectName(QStringLiteral("PrimaryButton"));
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
    button->setCursor(Qt::PointingHandCursor);
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
    const QString query = m_instanceSearchEdit ? m_instanceSearchEdit->text().trimmed() : QString();
    for (const Instance &instance : items) {
        const QString haystack = QStringLiteral("%1 %2 %3").arg(instance.name, instance.loader, instance.minecraftVersion.id);
        if (!query.isEmpty() && !haystack.contains(query, Qt::CaseInsensitive)) {
            continue;
        }

        auto *item = new QListWidgetItem;
        item->setData(Qt::UserRole, instance.id);
        item->setSizeHint(QSize(0, 74));
        m_instanceList->addItem(item);

        auto *row = new QFrame;
        row->setObjectName(QStringLiteral("InstanceRow"));
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(12, 10, 12, 10);
        rowLayout->setSpacing(12);

        auto *iconBadge = new QFrame;
        iconBadge->setObjectName(QStringLiteral("TinyIconBadge"));
        iconBadge->setFixedSize(42, 42);
        auto *iconLayout = new QVBoxLayout(iconBadge);
        iconLayout->setContentsMargins(9, 9, 9, 9);
        iconLayout->addWidget(makeIconLabel(QStringLiteral(":/icons/play.svg"), 24), 0, Qt::AlignCenter);
        rowLayout->addWidget(iconBadge);

        auto *textLayout = new QVBoxLayout;
        textLayout->setSpacing(2);
        auto *title = new QLabel(instance.name);
        title->setObjectName(QStringLiteral("InstanceTitle"));
        auto *meta = new QLabel(QStringLiteral("%1  /  %2").arg(instance.loader, instance.minecraftVersion.id));
        meta->setObjectName(QStringLiteral("InstanceMeta"));
        textLayout->addWidget(title);
        textLayout->addWidget(meta);
        rowLayout->addLayout(textLayout, 1);

        m_instanceList->setItemWidget(item, row);
    }
    if (m_instanceList->count() == 0) {
        auto *item = new QListWidgetItem;
        item->setData(Qt::UserRole, QString());
        item->setSizeHint(QSize(0, 74));
        m_instanceList->addItem(item);

        auto *row = new QFrame;
        row->setObjectName(QStringLiteral("InstanceRow"));
        auto *rowLayout = new QVBoxLayout(row);
        rowLayout->setContentsMargins(14, 12, 14, 12);
        rowLayout->setSpacing(2);
        auto *title = new QLabel(query.isEmpty() ? QStringLiteral("No profiles yet") : QStringLiteral("No matches"));
        title->setObjectName(QStringLiteral("InstanceTitle"));
        auto *meta = new QLabel(query.isEmpty() ? QStringLiteral("Install one from Library.") : QStringLiteral("Try another search."));
        meta->setObjectName(QStringLiteral("InstanceMeta"));
        rowLayout->addWidget(title);
        rowLayout->addWidget(meta);
        m_instanceList->setItemWidget(item, row);
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
