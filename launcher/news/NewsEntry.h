#pragma once

#include <QDomElement>
#include <QObject>
#include <QString>
#include <memory>

class NewsEntry : public QObject {
    Q_OBJECT

   public:
    /*!
     * Constructs an empty news entry.
     */
    explicit NewsEntry(QObject* parent = 0);

    /*!
     * Constructs a new news entry.
     * Note that content may contain HTML.
     */
    NewsEntry(const QString& title, const QString& content, const QString& link, QObject* parent = 0);

    /*!
     * Attempts to load information from the given XML element into the given news entry pointer.
     * If this fails, the function will return false and store an error message in the errorMsg pointer.
     */
    static bool fromXmlElement(const QDomElement& element, NewsEntry* entry, QString* errorMsg = 0);

    //! The post title.
    QString title;

    //! The post's content. May contain HTML.
    QString content;

    //! URL to the post.
    QString link;
};

using NewsEntryPtr = std::shared_ptr<NewsEntry>;
