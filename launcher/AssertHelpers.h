#pragma once

#if defined(ASSERT_NEVER)
#error ASSERT_NEVER already defined
#else
#define ASSERT_NEVER(cond) (Q_ASSERT((cond) == false), (cond))
#endif
