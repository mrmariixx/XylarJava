#include "Win7AeroTheme.h"

#include <QObject>
#include <QStyleFactory>

QString Win7AeroTheme::id()
{
    return "win7aero";
}

QString Win7AeroTheme::name()
{
    return QObject::tr("Windows 7 Aero");
}

QString Win7AeroTheme::tooltip()
{
    return QObject::tr("Classic Windows 7 Aero glass-inspired appearance");
}

QString Win7AeroTheme::qtTheme()
{
    const QStringList styles = QStyleFactory::keys();
    if (styles.contains("windowsvista"))
        return "windowsvista";
    if (styles.contains("windows11"))
        return "windows11";
    if (styles.contains("windows"))
        return "windows";
    return "Fusion";
}

QPalette Win7AeroTheme::colorScheme()
{
    QPalette palette;
    const QColor windowTop(230, 247, 252);
    const QColor windowBottom(204, 229, 244);
    const QColor base(255, 255, 255);
    const QColor alternate(238, 248, 251);
    const QColor buttonTop(252, 252, 252);
    const QColor buttonBottom(218, 231, 238);
    const QColor highlight(35, 151, 226);
    const QColor text(24, 35, 42);

    palette.setColor(QPalette::Window, windowTop);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, alternate);
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 225));
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, buttonBottom);
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0, 112, 184));
    palette.setColor(QPalette::Highlight, highlight);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, QColor(128, 128, 128));
    palette.setColor(QPalette::Light, buttonTop);
    palette.setColor(QPalette::Midlight, QColor(244, 250, 252));
    palette.setColor(QPalette::Mid, QColor(143, 174, 192));
    palette.setColor(QPalette::Dark, QColor(89, 123, 143));
    palette.setColor(QPalette::Shadow, QColor(44, 69, 84));

    Q_UNUSED(windowBottom);
    return fadeInactive(palette, fadeAmount(), fadeColor());
}

double Win7AeroTheme::fadeAmount()
{
    return 0.38;
}

QColor Win7AeroTheme::fadeColor()
{
    return QColor(230, 247, 252);
}

bool Win7AeroTheme::hasStyleSheet()
{
    return true;
}

QString Win7AeroTheme::appStyleSheet()
{
    return QStringLiteral(R"(
* {
    font-family: "Segoe UI", "Tahoma", sans-serif;
    font-size: 9pt;
}

QMainWindow, QDialog, QWidget {
    background-color: #E6F7FC;
    color: #18232A;
}

QToolBar {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.18 #F7FDFF, stop:0.5 #D7F0FA, stop:1 #AFCFE4);
    border: 1px solid #89B6CE;
    border-top-color: #FFFFFF;
    spacing: 4px;
    padding: 3px;
}

QToolBar::separator {
    background: #89B6CE;
    width: 1px;
    margin: 4px 6px;
}

QStatusBar {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F7FDFF, stop:0.45 #DDF3FA, stop:1 #B4D4E7);
    border-top: 1px solid #89B6CE;
}

QMenuBar {
    background: transparent;
    border-bottom: 1px solid #89B6CE;
    padding: 2px;
}

QMenuBar::item {
    background: transparent;
    padding: 4px 10px;
    border-radius: 3px;
}

QMenuBar::item:selected {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F7FDFF, stop:0.45 #C9F5FF, stop:1 #82C7EF);
    border: 1px solid #2A9DDA;
}

QMenu {
    background-color: #FBFEFF;
    border: 1px solid #8FAFC0;
    padding: 4px;
}

QMenu::item {
    padding: 5px 28px 5px 24px;
    border: 1px solid transparent;
}

QMenu::item:selected {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F1FBFF, stop:0.48 #C9F5FF, stop:1 #A0DDF3);
    border: 1px solid #2A9DDA;
}

QPushButton, QToolButton {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.42 #F7FDFF, stop:0.5 #E7F1F5, stop:1 #C8DCE7);
    border: 1px solid #6D93AA;
    border-top-color: #BFEFFF;
    border-radius: 4px;
    padding: 4px 14px;
    min-height: 20px;
    color: #18232A;
}

QPushButton:hover, QToolButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.28 #E8FDFF, stop:0.5 #B9F0FF, stop:1 #78C8EF);
    border: 1px solid #168DD0;
}

QPushButton:pressed, QToolButton:pressed {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #A7E5F7, stop:0.49 #82D6F2, stop:0.5 #5AB9E6, stop:1 #2A8EC9);
    border: 1px solid #006FA8;
    padding-top: 5px;
    padding-bottom: 3px;
}

QPushButton:default {
    border: 1px solid #006FA8;
}

QPushButton:disabled, QToolButton:disabled {
    color: #A0A0A0;
    background: #F4F4F4;
    border: 1px solid #C0C0C0;
}

QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: #FFFFFF;
    border: 1px solid #8FAFC0;
    border-radius: 3px;
    padding: 3px 5px;
    selection-background-color: #2397E2;
    selection-color: #FFFFFF;
}

QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #1C9DD8;
}

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 18px;
    border-left: 1px solid #8FAFC0;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.45 #F0FCFF, stop:1 #C8DCE7);
}

QComboBox QAbstractItemView {
    background: #FFFFFF;
    border: 1px solid #8FAFC0;
    selection-background-color: #2397E2;
    selection-color: #FFFFFF;
}

QGroupBox {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.18 #F8FEFF, stop:0.58 #E1F5FA, stop:1 #C7E2EF);
    border: 1px solid #89B6CE;
    border-top-color: #FFFFFF;
    border-radius: 5px;
    margin-top: 12px;
    padding: 12px 8px 8px 8px;
    font-weight: bold;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 10px;
    padding: 0 4px;
    color: #18232A;
}

QTabWidget::pane {
    background: #F8FEFF;
    border: 1px solid #89B6CE;
    border-top: none;
    top: -1px;
}

QTabBar::tab {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.35 #F1FBFF, stop:1 #BEDCEC);
    border: 1px solid #89B6CE;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    padding: 6px 14px;
    margin-right: 2px;
    min-width: 70px;
}

QTabBar::tab:selected {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.35 #FFFFFF, stop:1 #F2FBFE);
    border-bottom: 1px solid #F8FEFF;
    margin-bottom: -1px;
}

QTabBar::tab:hover:!selected {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.45 #D8FAFF, stop:1 #A8DFF3);
}

QHeaderView::section {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.42 #EFFBFF, stop:1 #BFDDEC);
    border: 1px solid #89B6CE;
    padding: 4px 6px;
    font-weight: bold;
}

QTreeView, QListView, QTableView {
    background: #FFFFFF;
    alternate-background-color: #EEF9FC;
    border: 1px solid #89B6CE;
    selection-background-color: #2397E2;
    selection-color: #FFFFFF;
    gridline-color: #C8E2EF;
}

QTreeView::item:hover, QListView::item:hover, QTableView::item:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F7FDFF, stop:1 #D8FAFF);
}

QScrollBar:vertical {
    background: #E7F5FA;
    width: 17px;
    margin: 0;
    border: 1px solid #89B6CE;
}

QScrollBar::handle:vertical {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #FFFFFF, stop:0.45 #F0FCFF, stop:1 #BFDDEC);
    min-height: 24px;
    border: 1px solid #6D93AA;
    border-radius: 2px;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #C8DCE7);
    border: 1px solid #6D93AA;
    height: 17px;
}

QScrollBar:horizontal {
    background: #E7F5FA;
    height: 17px;
    margin: 0;
    border: 1px solid #89B6CE;
}

QScrollBar::handle:horizontal {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.45 #F0FCFF, stop:1 #BFDDEC);
    min-width: 24px;
    border: 1px solid #6D93AA;
    border-radius: 2px;
}

QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #C8DCE7);
    border: 1px solid #6D93AA;
    width: 17px;
}

QProgressBar {
    background: #FFFFFF;
    border: 1px solid #89B6CE;
    border-radius: 3px;
    text-align: center;
    color: #18232A;
}

QProgressBar::chunk {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #B7F8B1, stop:0.18 #75E57D, stop:0.5 #32BFE8, stop:1 #168DD0);
    border-radius: 2px;
}

QCheckBox::indicator, QRadioButton::indicator {
    width: 13px;
    height: 13px;
    border: 1px solid #6D93AA;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #C8DCE7);
}

QCheckBox::indicator:checked {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #D8FAFF, stop:0.55 #75E57D, stop:1 #2397E2);
    border: 1px solid #006FA8;
}

QSlider::groove:horizontal {
    height: 6px;
    background: #FFFFFF;
    border: 1px solid #8FAFC0;
    border-radius: 3px;
}

QSlider::handle:horizontal {
    width: 12px;
    margin: -5px 0;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:0.45 #E8FDFF, stop:1 #95D9F2);
    border: 1px solid #6D93AA;
    border-radius: 2px;
}

QToolTip {
    color: #18232A;
    background-color: #FFFFE8;
    border: 1px solid #8FAFC0;
    padding: 4px;
}

QSplitter::handle {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #C7E2EF, stop:0.5 #89B6CE, stop:1 #C7E2EF);
}

QFrame[frameShape="4"], QFrame[frameShape="5"] {
    color: #89B6CE;
}
)");
}
