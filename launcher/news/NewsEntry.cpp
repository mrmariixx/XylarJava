#include "NewsEntry.h"

#include <QDomNodeList>
#include <QVariant>

NewsEntry::NewsEntry(QObject* parent) : QObject(parent)
{
    this->title = tr("Untitled");
    this->content = tr("No content.");
    this->link = "";
}

NewsEntry::NewsEntry(const QString& title, const QString& content, const QString& link, QObject* parent) : QObject(parent)
{
    this->title = title;
    this->content = content;
    this->link = link;
}

/*!
 * Gets the text content of the given child element as a QVariant.
 */
inline QString childValue(const QDomElement& element, const QString& childName, QString defaultVal = "")
{
    QDomNodeList nodes = element.elementsByTagName(childName);
    if (nodes.count() > 0) {
        QDomElement elem = nodes.at(0).toElement();
        return elem.text();
    } else {
        return defaultVal;
    }
}

bool NewsEntry::fromXmlElement(const QDomElement& element, NewsEntry* entry, [[maybe_unused]] QString* errorMsg)
{
    QString title = childValue(element, "title", tr("Untitled"));
    QString content = childValue(element, "content", tr("No content."));
    QString link = childValue(element, "id");

    entry->title = title;
    entry->content = content;
    entry->link = link;
    return true;
}
