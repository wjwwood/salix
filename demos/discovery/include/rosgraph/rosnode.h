/**
 * @file
 *
 * @ingroup rosgraph
 *
 * @brief API for rosnode's in rosgraph's
 *
 * This file contains the API for the rosnode type which represents nodes
 * in a rosgraph. This API can be used to create and free rosnode's,
 * add them to a rosgraph, and other node realted tasks.
 */

/*
 * Copyright 2013 Open Source Robotics Foundation, Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef ROSGRAPH_ROSNODE_H
#define ROSGRAPH_ROSNODE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uint32_t rosnode_handle_t;

struct rosnode_struct {
    char *name;
};

void
ros_create_rosnode(rosnode_handle_t *handle);

void
ros_free_rosnode(rosnode_handle_t *handle);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ROSGRAPH_ROSNODE_H */
