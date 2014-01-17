ROS Concepts
============

Nodes
-----
Nodes can be defined by Four things:

- Name:
 - String which has to be unique in the graph.
- Machine ID:
 - Unique ID for the machine the node's process is running on.
- Process ID:
 - Unix like process ID of the process that the node runs in.
 - Nodes can share a process ID.
- Node ID:
 - Used to differentiate nodes which share a process ID.

Before any nodes can be "started" the containing process must first join the ROS Graph.

Joining the ROS Graph
^^^^^^^^^^^^^^^^^^^^^
To join the ROS Graph a process must join the multicast UDP group and create a ROS graph instance which will maintain a representation of the current ROS graph. This only has to be done once per process. The ``ROS_GRAPH_URL`` environment variable is used to get the multicast UDP group address and port.

Then the blocking call ``ros_graph_spin`` must be called, it performs these functions:

- Create a ROS graph instance (a data structure which models the current ROS graph).
- Calls ``recvfrom`` on the socket and processes incoming ROS Graph messages.
- Processed messages update the ROS graph instance.
- Sends out ROS Graph messages on the behalf of the nodes of the process.

Once this is running a node can be created.

Creating a Node
^^^^^^^^^^^^^^^

When a node starts up (``ros_create_node`` is called), it follows these procedures:

- Get process ID
- Get node ID
- Join the multicast UDP group using the address and port from the ROS_GRAPH_URL

Once the node is initialized it needs to start listening to graph messages before joining the ROS graph. This is done by running the ``ros_graph_spin`` command. ``ros_graph_spin`` is a blocking call which runs until the node is shutdown. command
- Send a ROS_GRAPH_MSG_DISC_REQ  message

