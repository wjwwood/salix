#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define ROS_ECOSYSTEM_VAR_NAME "ROS_ECOSYSTEM_ID"

#define ROS_TOPO_MSG_HEART_BEAT 0x00
#define ROS_TOPO_MSG_UP         0x10
#define ROS_TOPO_MSG_DOWN       0x11
#define ROS_TOPO_MSG_DISC_REQ   0x20
#define ROS_TOPO_MSG_DISC_RSP   0x21

#define HELLO_PORT 11311
#define HELLO_GROUP "225.82.79.83"

#pragma pack(1)

struct broadcast_message {
    uint32_t ecosystem_id;
    int32_t process_id;
    uint32_t node_id;
    char msg_type;
};

#pragma pack(0)

// static uint32_t this_node_id = 0;

uint32_t
create_node_id(int32_t process_id) {
    return process_id % 10;
}

void
get_time_now (struct timespec *time)
{
# ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  time->tv_sec = mts.tv_sec;
  time->tv_nsec = mts.tv_nsec;
# else
  clock_gettime(CLOCK_REALTIME, &time);
# endif
}

void
diff_timespec (struct timespec *start, struct timespec *end, struct timespec *result)
{
  if (start->tv_sec > end->tv_sec)
  {
    perror("diffing timespecs");
  }
  result->tv_sec = end->tv_sec - start->tv_sec;
  result->tv_nsec = end->tv_nsec - start->tv_nsec;
  if (result->tv_nsec < 0)
  {
    result->tv_nsec = 1e9 - result->tv_nsec;
    result->tv_sec -= 1;
  }
}

struct sockaddr_in addr, addr2;
int fd;

uint32_t
get_ecosystem_id()
{
    char * value = getenv(ROS_ECOSYSTEM_VAR_NAME);
    if (!value)
    {
        return 1;
    }
    return atoi(value);
}

static void
shutdown_handler (int signal)
{
    struct broadcast_message bm_out;
    bm_out.ecosystem_id = get_ecosystem_id();
    bm_out.process_id = getpid();
    bm_out.node_id = create_node_id(bm_out.process_id);
    bm_out.msg_type = ROS_TOPO_MSG_DOWN;
    if (sendto(fd, &bm_out, sizeof(bm_out), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("sendto");
        exit(1);
    }
    exit(0);
}

int
main(int argc, char *argv[]) {
    if (signal(SIGINT, shutdown_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }
    struct broadcast_message bm, bm_out;
    bm_out.ecosystem_id = get_ecosystem_id();
    bm_out.process_id = getpid();
    printf("The process id of this node is [%d]\n", bm_out.process_id);
    bm_out.node_id = create_node_id(bm_out.process_id);
    bm_out.msg_type = ROS_TOPO_MSG_UP;

    struct ip_mreq mreq;

    /* create what looks like an ordinary UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* make socket non-blocking */
    int nonBlocking = 1;
    if (fcntl(fd, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
        perror("Non-Blocking");
    }

    /* allow multiple sockets to use the same PORT number */
    u_int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
        perror("setsockopt (SOL_SOCKET, SO_REUSEPORT)");
    }

    /* set up destination address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(HELLO_GROUP);
    addr.sin_port = htons(HELLO_PORT);

    /* set up receiver address */
    memset(&addr2, 0, sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
    addr2.sin_port = htons(HELLO_PORT);

    /* bind to receive address */
    if (bind(fd, (struct sockaddr *) &addr2, sizeof(addr2)) < 0) {
        perror("bind");
        exit(1);
    }

    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(HELLO_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    struct timespec last_update, now, result;
    int first_time = 1;
    while (1) {
        if (first_time)
        {
            get_time_now(&last_update);
        }
        get_time_now(&now);
        diff_timespec(&last_update, &now, &result);
        /* Every ~1s publish a heartbeat */
        if (first_time || result.tv_sec)
        {
            // float delta = result.tv_sec + ((float)result.tv_nsec / 1e9);
            // printf("Time since last heartbeat [%f]\n", delta);
            if (sendto(fd, &bm_out, sizeof(bm_out), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0)
            {
                perror("sendto");
                exit(1);
            }
            bm_out.msg_type = ROS_TOPO_MSG_HEART_BEAT;
            first_time = 0;
            get_time_now(&last_update);
        }
        /* Everyloop ask for some data */
        socklen_t len_addr2 = sizeof(addr2);
        if (recvfrom(fd, &bm, sizeof(bm), 0, (struct sockaddr *) &addr2, &len_addr2) < 0)
        {
            if (errno == EAGAIN)
            {
                continue;
            }
            perror("recvfrom");
            exit(1);
        }

        /* If process_id is 0 it should be asking for discovery */
        if (bm.process_id == 0)
        {
            if (bm.msg_type == ROS_TOPO_MSG_DISC_REQ)
            {
                bm_out.msg_type = ROS_TOPO_MSG_DISC_RSP;
                if (sendto(fd, &bm_out, sizeof(bm_out), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0)
                {
                    perror("sendto");
                    exit(1);
                }
            }
        }
        /* If not 0 or our pid, someone else is talking to us! */
        else if (bm.process_id != getpid())
        {
            switch (bm.msg_type)
            {
                case ROS_TOPO_MSG_UP:
                    printf("[%d] Node [%d] with process id [%d] just started\n",
                           bm.ecosystem_id, bm.node_id, bm.process_id);
                    break;
                case ROS_TOPO_MSG_DOWN:
                    printf("[%d] Node [%d] with process id [%d] just shutdown\n",
                           bm.ecosystem_id, bm.node_id, bm.process_id);
                    break;
                case ROS_TOPO_MSG_HEART_BEAT:
                    printf("[%d] Heartbeat from node [%d] with process id [%d]\n",
                           bm.ecosystem_id, bm.node_id, bm.process_id);
                    break;
                case ROS_TOPO_MSG_DISC_RSP:
                    printf("[%d] Discovery response from node [%d] with process id [%d]\n",
                           bm.ecosystem_id, bm.node_id, bm.process_id);
                default:
                    printf("[%d] Unkown event [%d] received from node [%d] with process id [%d]\n",
                           bm.ecosystem_id, bm.msg_type, bm.node_id, bm.process_id);
            }
        }

        usleep(1000);
    }

    return 0;
}
