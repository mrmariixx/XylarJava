#pragma once

#include <QString>
#include <QStringList>
#include <QUrl>

struct ModLicense {
    QString name = {};
    QString id = {};
    QString url = {};
    QString description = {};

    ModLicense() {}

    ModLicense(const QString license)
    {
        // FIXME: come up with a better license parsing.
        // handle SPDX identifiers? https://spdx.org/licenses/
        auto parts = license.split(' ');
        QStringList notNameParts = {};
        for (auto part : parts) {
            auto _url = QUrl(part);
            if (part.startsWith("(") && part.endsWith(")"))
                _url = QUrl(part.mid(1, part.size() - 2));

            if (_url.isValid() && !_url.scheme().isEmpty() && !_url.host().isEmpty()) {
                this->url = _url.toString();
                notNameParts.append(part);
                continue;
            }
        }

        for (auto part : notNameParts) {
            parts.removeOne(part);
        }

        auto licensePart = parts.join(' ');
        this->name = licensePart;
        this->description = licensePart;

        if (parts.size() == 1) {
            this->id = parts.first();
        }
    }

    ModLicense(const QString& name_, const QString& id_, const QString& url_, const QString& description_)
        : name(name_), id(id_), url(url_), description(description_)
    {}

    ModLicense(const ModLicense& other) : name(other.name), id(other.id), url(other.url), description(other.description) {}

    ModLicense& operator=(const ModLicense& other)
    {
        this->name = other.name;
        this->id = other.id;
        this->url = other.url;
        this->description = other.description;

        return *this;
    }

    ModLicense& operator=(const ModLicense&& other)
    {
        this->name = other.name;
        this->id = other.id;
        this->url = other.url;
        this->description = other.description;

        return *this;
    }

    bool isEmpty() { return this->name.isEmpty() && this->id.isEmpty() && this->url.isEmpty() && this->description.isEmpty(); }
};

struct ModDetails {
    /* Mod ID as defined in the ModLoader-specific metadata */
    QString mod_id = {};

    /* Human-readable name */
    QString name = {};

    /* Human-readable mod version */
    QString version = {};

    /* Human-readable minecraft version */
    QString mcversion = {};

    /* URL for mod's home page */
    QString homeurl = {};

    /* Human-readable description */
    QString description = {};

    /* List of the author's names */
    QStringList authors = {};

    /* Issue Tracker URL */
    QString issue_tracker = {};

    /* License */
    QList<ModLicense> licenses = {};

    /* Path of mod logo */
    QString icon_file = {};

    QStringList dependencies = {};

    ModDetails() = default;

    /** Metadata should be handled manually to properly set the mod status. */
    ModDetails(const ModDetails& other)
        : mod_id(other.mod_id)
        , name(other.name)
        , version(other.version)
        , mcversion(other.mcversion)
        , homeurl(other.homeurl)
        , description(other.description)
        , authors(other.authors)
        , issue_tracker(other.issue_tracker)
        , licenses(other.licenses)
        , icon_file(other.icon_file)
        , dependencies(other.dependencies)
    {}

    ModDetails& operator=(const ModDetails& other) = default;

    ModDetails& operator=(ModDetails&& other) = default;
};
