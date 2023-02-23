// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef IOX_HOOFS_UNIX_PLATFORM_SEMAPHORE_HPP
#define IOX_HOOFS_UNIX_PLATFORM_SEMAPHORE_HPP

#include <semaphore.h>

using reload_sem_t = sem_t;

inline int reload_sem_getvalue(reload_sem_t* sem, int* sval)
{
    return sem_getvalue(sem, sval);
}

inline int reload_sem_post(reload_sem_t* sem)
{
    return sem_post(sem);
}

inline int reload_sem_wait(reload_sem_t* sem)
{
    return sem_wait(sem);
}

inline int reload_sem_trywait(reload_sem_t* sem)
{
    return sem_trywait(sem);
}

inline int reload_sem_timedwait(reload_sem_t* sem, const struct timespec* abs_timeout)
{
    return sem_timedwait(sem, abs_timeout);
}

inline int reload_sem_close(reload_sem_t* sem)
{
    return sem_close(sem);
}

inline int reload_sem_destroy(reload_sem_t* sem)
{
    return sem_destroy(sem);
}

inline int reload_sem_init(reload_sem_t* sem, int pshared, unsigned int value)
{
    return sem_init(sem, pshared, value);
}

template <typename... Targs>
inline reload_sem_t* reload_sem_open(const char* name, int oflag, Targs... args)
{
    return sem_open(name, oflag, args...);
}

inline int reload_sem_unlink(const char* name)
{
    return sem_unlink(name);
}

#endif // IOX_HOOFS_UNIX_PLATFORM_SEMAPHORE_HPP
