#pragma once

#include <QList>
#include <QVariant>

namespace QVariantUtils {

template <typename T>
inline QList<T> toList(QVariant src)
{
    QVariantList variantList = src.toList();

    QList<T> list_t;
    list_t.reserve(variantList.size());
    for (const QVariant& v : variantList) {
        list_t.append(v.value<T>());
    }
    return list_t;
}

template <typename T>
inline QVariant fromList(QList<T> val)
{
    QVariantList variantList;
    variantList.reserve(val.size());
    for (const T& v : val) {
        variantList.append(v);
    }

    return variantList;
}

}  // namespace QVariantUtils
