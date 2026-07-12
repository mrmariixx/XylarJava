#pragma once

#include <QString>

namespace Time {

QString prettifyDuration(int64_t duration, bool noDays = false);

/**
 * @brief Returns a string with short form time duration ie. `2days 1h3m4s56.0ms`.
 * miliseconds are only included if `precision` is greater than 0.
 *
 * @param duration a number of seconds as floating point
 * @param precision number of decmial points to display on fractons of a second, defualts to 0.
 * @return QString
 */
QString humanReadableDuration(double duration, int precision = 0);
}  // namespace Time
