#include "instances/Instance.h"

namespace xylar {

bool Instance::isValid() const
{
    return !id.isEmpty() && !name.isEmpty() && !minecraftVersion.id.isEmpty();
}

} // namespace xylar
