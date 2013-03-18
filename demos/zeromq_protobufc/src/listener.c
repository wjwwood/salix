#include <stdio.h>
#include <stdlib.h>

#include <zmq.h>

#include "string_stamped.pb-c.h"


int main(int argc, const char * argv[]) {
  void *context = zmq_ctx_new();
  printf("Connecting...\n");
  void *subscriber = zmq_socket(context, ZMQ_SUB);
  int rc = zmq_connect(subscriber, "tcp://localhost:1234");
  assert(rc == 0);
  printf("Connected!\n");

  assert (zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0) == 0);

  StringStamped *msg;

  int i = 0;
  while (i < 10) {
    // Read packed message from standard-input.
    zmq_msg_t zmq_msg;
    assert (zmq_msg_init(&zmq_msg) == 0);
    printf("Reading...\n");
    int bytes_read = zmq_msg_recv(&zmq_msg, subscriber, 0);
    printf("Read %i bytes.\n", bytes_read);
    if (bytes_read <= 0) {
      zmq_msg_close(&zmq_msg);
      continue;
    }

    // Unpack the message using protobuf-c.
    msg = string_stamped__unpack(NULL, bytes_read, zmq_msg_data(&zmq_msg));
    if (msg == NULL) {
      fprintf(stderr, "error unpacking incoming message\n");
      string_stamped__free_unpacked(msg, NULL);
      zmq_msg_close(&zmq_msg);
      continue;
    }

    // display the message's fields.
    printf("Received: %s", msg->msg);  // required field
    printf(" at secs: %i, nsecs: %i", msg->stamp->secs, msg->stamp->nsecs);
    printf("\n");

    // Free the unpacked message
    string_stamped__free_unpacked(msg, NULL);
    zmq_msg_close(&zmq_msg);
    i++;
    sleep(1);
  }
  return 0;
}
