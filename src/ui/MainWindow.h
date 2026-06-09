#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QString>

#include "launcher/LauncherController.h"

class QFrame;
class QLabel;
class QComboBox;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;

namespace xylar {

class XylarNavBar;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *createHomePage();
    QWidget *createModpacksPage();
    QWidget *createSettingsPage();

    QFrame *createPanel(const QString &objectName);
    QLabel *createSectionTitle(const QString &text);
    QLabel *createMutedText(const QString &text);
    QPushButton *createActionButton(const QString &text, const QString &iconResource);

    void refreshVersionList();
    void refreshInstanceList();
    void appendLog(const QString &line);
    [[nodiscard]] QString selectedVersionId() const;
    [[nodiscard]] QString selectedInstanceId() const;
    [[nodiscard]] QString selectedLoader() const;
    [[nodiscard]] QString effectiveJavaPath() const;

    LauncherController m_controller;
    QStackedWidget *m_pages = nullptr;
    XylarNavBar *m_navBar = nullptr;
    QComboBox *m_versionCombo = nullptr;
    QComboBox *m_versionFilterCombo = nullptr;
    QComboBox *m_loaderCombo = nullptr;
    QListWidget *m_instanceList = nullptr;
    QLineEdit *m_playerNameEdit = nullptr;
    QLineEdit *m_instanceNameEdit = nullptr;
    QLineEdit *m_javaPathEdit = nullptr;
    QSpinBox *m_minMemorySpin = nullptr;
    QSpinBox *m_maxMemorySpin = nullptr;
    QProgressBar *m_progress = nullptr;
    QPlainTextEdit *m_log = nullptr;
};

} // namespace xylar
