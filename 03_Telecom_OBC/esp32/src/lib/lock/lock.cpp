#include "lock.hpp"
#include "../../common/logger.hpp"

Lock::Lock(void) {
    semaphore = xSemaphoreCreateMutex();
}

Lock::~Lock(void) {
}

int Lock::lock(void) {
    if (xSemaphoreTake(semaphore, (TickType_t)10000) == pdTRUE) {
        return 0;
    } else {
        logger_error("lock failed\n");
        return 1;
    }
}

void Lock::unlock(void) {
    xSemaphoreGive(semaphore);
}
