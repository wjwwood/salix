/**
 * @file
 *
 * @ingroup rosgraph
 *
 * @brief API for the rosgraph
 *
 * This file contains the API for the rosgraph instance. This API can be used
 * to interact with the ROS graph by adding nodes, quering states,
 * registering callbacks, and more.
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

#ifndef ROSGRAPH_ROSGRAPH_H
#define ROSGRAPH_ROSGRAPH_H

#include <stdint.h>

#include "rosnode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct rosgraph_struct {
    char *address;
    uint32_t port;
    rosnode_struct *nodes;
};

int
ros_start_rosgraph();

int
ros_shutdown_rosgraph();

int
ros_add_node_to_graph(rosnode_handle_t *node_handle);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ROSGRAPH_ROSGRAPH_H */
