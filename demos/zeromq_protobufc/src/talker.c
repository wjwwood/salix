#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef HAS_CLOCK_GETTIME
# include <sys/time.h>
#endif

#include <zmq.h>

#include "string_stamped.pb-c.h"
#include "time.pb-c.h"

void get_time(Time *t) {
#if HAS_CLOCK_GETTIME
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    t->secs  = start.tv_sec;
    t->nsecs = start.tv_nsec;
#else
    struct timeval timeofday;
    gettimeofday(&timeofday,NULL);
    t->secs  = timeofday.tv_sec;
    t->nsecs = timeofday.tv_usec * 1000;
#endif
}

int main (int argc, const char * argv[]) {
    void *context = zmq_ctx_new();

    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:1234");

    StringStamped msg = STRING_STAMPED__INIT;
    Time time_msg = TIME__INIT;
    void *buf;
    unsigned len;

    if (argc != 2) {
        fprintf(stderr,"usage: demo <string>\n");
        return 1;
    }

    int i = 0;
    while (i < 10) {
        msg.msg = argv[1];
        get_time(&time_msg);
        msg.stamp = &time_msg;
        len = string_stamped__get_packed_size(&msg);

        zmq_msg_t zmq_msg;
        assert (zmq_msg_init_size(&zmq_msg, len) == 0);
        string_stamped__pack(&msg, zmq_msg_data(&zmq_msg));

        fprintf(stderr, "Writing %d serialized bytes\n", len); // See the length of message
        zmq_msg_send(&zmq_msg, publisher, 0);

        assert (zmq_msg_close(&zmq_msg) == 0);
        i++;
        sleep(1);
    }
    zmq_close(publisher);
    zmq_ctx_destroy(context);

    fprintf(stderr, "Talker done.\n");
    return 0;
}
