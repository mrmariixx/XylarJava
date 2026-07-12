#include "AtlPage.h"
#include "ui/widgets/ProjectItem.h"
#include "ui_AtlPage.h"

#include "BuildConfig.h"
#include "StringUtils.h"

#include "AtlUserInteractionSupportImpl.h"
#include "modplatform/atlauncher/ATLPackInstallTask.h"
#include "ui/dialogs/NewInstanceDialog.h"

#include <QMessageBox>

AtlPage::AtlPage(NewInstanceDialog* dialog, QWidget* parent) : QWidget(parent), ui(new Ui::AtlPage), dialog(dialog)
{
    ui->setupUi(this);

    filterModel = new Atl::FilterModel(this);
    listModel = new Atl::ListModel(this);
    filterModel->setSourceModel(listModel);
    ui->packView->setModel(filterModel);
    ui->packView->setSortingEnabled(true);

    ui->packView->header()->hide();
    ui->packView->setIndentation(0);

    ui->versionSelectionBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->versionSelectionBox->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->versionSelectionBox->view()->parentWidget()->setMaximumHeight(300);

    for (int i = 0; i < filterModel->getAvailableSortings().size(); i++) {
        ui->sortByBox->addItem(filterModel->getAvailableSortings().keys().at(i));
    }
    ui->sortByBox->setCurrentText(filterModel->translateCurrentSorting());

    connect(ui->searchEdit, &QLineEdit::textChanged, this, &AtlPage::triggerSearch);
    connect(ui->sortByBox, &QComboBox::currentTextChanged, this, &AtlPage::onSortingSelectionChanged);
    connect(ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &AtlPage::onSelectionChanged);
    connect(ui->versionSelectionBox, &QComboBox::currentTextChanged, this, &AtlPage::onVersionSelectionChanged);

    ui->packView->setItemDelegate(new ProjectItemDelegate(this));
}

AtlPage::~AtlPage()
{
    delete ui;
}

bool AtlPage::shouldDisplay() const
{
    return true;
}

void AtlPage::retranslate()
{
    ui->retranslateUi(this);
}

void AtlPage::openedImpl()
{
    if (!initialized) {
        listModel->request();
        initialized = true;
    }

    suggestCurrent();
}

void AtlPage::suggestCurrent()
{
    if (!isOpened) {
        return;
    }

    if (selectedVersion.isEmpty()) {
        dialog->setSuggestedPack();
        return;
    }

    auto uiSupport = new AtlUserInteractionSupportImpl(this);
    dialog->setSuggestedPack(selected.name, selectedVersion, new ATLauncher::PackInstallTask(uiSupport, selected.name, selectedVersion));

    auto editedLogoName = "atl_" + selected.safeName;
    auto url = QString(BuildConfig.ATL_DOWNLOAD_SERVER_URL + "launcher/images/%1").arg(selected.safeName);
    listModel->getLogo(selected.safeName, url,
                       [this, editedLogoName](QString logo) { dialog->setSuggestedIconFromFile(logo, editedLogoName); });
}

void AtlPage::triggerSearch()
{
    filterModel->setSearchTerm(ui->searchEdit->text());
}

void AtlPage::onSortingSelectionChanged(QString sort)
{
    auto toSet = filterModel->getAvailableSortings().value(sort);
    filterModel->setSorting(toSet);
}

void AtlPage::onSelectionChanged(QModelIndex first, [[maybe_unused]] QModelIndex second)
{
    ui->versionSelectionBox->clear();

    if (!first.isValid()) {
        if (isOpened) {
            dialog->setSuggestedPack();
        }
        return;
    }

    QVariant raw = filterModel->data(first, Qt::UserRole);
    Q_ASSERT(raw.canConvert<ATLauncher::IndexedPack>());
    selected = raw.value<ATLauncher::IndexedPack>();

    ui->packDescription->setHtml(StringUtils::htmlListPatch(selected.description.replace("\n", "<br>")));

    for (const auto& version : selected.versions) {
        ui->versionSelectionBox->addItem(version.version);
    }

    suggestCurrent();
}

void AtlPage::onVersionSelectionChanged(QString version)
{
    if (version.isNull() || version.isEmpty()) {
        selectedVersion = "";
        return;
    }

    selectedVersion = version;
    suggestCurrent();
}

void AtlPage::setSearchTerm(QString term)
{
    ui->searchEdit->setText(term);
}

QString AtlPage::getSerachTerm() const
{
    return ui->searchEdit->text();
}
