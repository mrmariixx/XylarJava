#pragma once

#include <QModelIndex>

#include "TechnicData.h"
#include "net/NetJob.h"

namespace Technic {

using LogoCallback = std::function<void(QString)>;

class ListModel : public QAbstractListModel {
    Q_OBJECT

   public:
    ListModel(QObject* parent);
    virtual ~ListModel();

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual int rowCount(const QModelIndex& parent) const;

    void getLogo(const QString& logo, const QString& logoUrl, LogoCallback callback);
    void searchWithTerm(const QString& term);

    bool hasActiveSearchJob() const { return jobPtr && jobPtr->isRunning(); }
    Task::Ptr activeSearchJob() { return hasActiveSearchJob() ? jobPtr : nullptr; }

   private slots:
    void searchRequestFinished(QByteArray* responsePtr);
    void searchRequestFailed();

    void logoFailed(QString logo);
    void logoLoaded(QString logo, QString out);

   private:
    void performSearch();
    void requestLogo(QString logo, QString url);

   private:
    QList<Modpack> modpacks;
    QStringList m_failedLogos;
    QStringList m_loadingLogos;
    QMap<QString, QIcon> m_logoMap;
    QMap<QString, LogoCallback> waitingCallbacks;

    QString currentSearchTerm;
    enum SearchState { None, ResetRequested, Finished } searchState = None;
    enum SearchMode {
        List,
        Single,
    } searchMode = List;
    NetJob::Ptr jobPtr;
};

}  // namespace Technic
