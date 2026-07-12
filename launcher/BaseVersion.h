#pragma once

#include <QMetaType>
#include <QString>
#include <memory>

/*!
 * An abstract base class for versions.
 */
class BaseVersion {
   public:
    // TODO: delete
    using Ptr = std::shared_ptr<BaseVersion>;
    virtual ~BaseVersion() {}
    /*!
     * A string used to identify this version in config files.
     * This should be unique within the version list or shenanigans will occur.
     */
    virtual QString descriptor() const = 0;

    /*!
     * The name of this version as it is displayed to the user.
     * For example: "1.5.1"
     */
    virtual QString name() const = 0;

    /*!
     * This should return a string that describes
     * the kind of version this is (Stable, Beta, Snapshot, whatever)
     */
    virtual QString typeString() const = 0;
    virtual bool operator<(BaseVersion& a) const { return name() < a.name(); }
    virtual bool operator>(BaseVersion& a) const { return name() > a.name(); }
};

Q_DECLARE_METATYPE(BaseVersion::Ptr)
