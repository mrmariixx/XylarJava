#include "QuitAfterGameStop.h"
#include <launch/LaunchTask.h>
#include "Application.h"

void QuitAfterGameStop::executeTask()
{
    APPLICATION->quit();
}
