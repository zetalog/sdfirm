#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

//#define DEBUG

#define __unused		__attribute__((__unused__))

typedef struct {
	unsigned char dest_addr[6];
	unsigned char src_addr[6];
	unsigned char type[2];
} eth_hdr;
#define ETH_HDR_SIZE		sizeof(eth_hdr)

#define NUM_OF_PACKETS	10
#define FRAME_SIZE	80

static __u8 frame[FRAME_SIZE];
static int sock = -1;
static __u8 ETH_TYPE[2] = { 0x99, 0x99 };
static __u8 payload[] = { 'V', 'Y', 'A' };
static char *ifname = NULL;
static int ifindex;
static char ifhwaddr[ETH_ALEN];

static void close_sock(void)
{
	if (sock >= 0) {
		close(sock);
		sock = -1;
	}
}

static int open_sock(void)
{
	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0) {
		fprintf(stderr, "%s: socket(AF_PACKET): %s\n",
			ifname, strerror(errno));
		return -EFAULT;
	}
	return 0;
}

static int get_ifhwaddr(char *hwaddr)
{
	struct ifreq ifr;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	if (ret < 0) {
		fprintf(stderr, "%s: ioctl(SIOCGIFHWADDR): %s\n",
			ifname, strerror(errno));
		return ret;
	}
	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	return 0;
}

static int get_ifindex(int *index)
{
	struct ifreq ifr;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ret = ioctl(sock, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		fprintf(stderr, "%s: ioctl(SIOCGIFINDEX): %s\n",
			ifname, strerror(errno));
		return ret;
	}
	*index = ifr.ifr_ifindex;
	return 0;
}

static int loopback_recv_data(int len)
{
	eth_hdr *hdr = (eth_hdr *)frame;
	__u8 *ptr;
	int i;

	if (len < FRAME_SIZE) {
		fprintf(stderr, "%s: bad packet length %d\n",
			ifname, len);
		return 1;
	}
	if (hdr->type[0] != ETH_TYPE[0] || hdr->type[1] != ETH_TYPE[1]) {
		fprintf(stderr, "%s: bad packet type %02x%02x\n",
			ifname, hdr->type[0], hdr->type[1]);
		return 1;
	}

	ptr = frame + ETH_HDR_SIZE;
	len -= ETH_HDR_SIZE;
	for (i = 0; i < len; i++) {
		if (ptr[i] != payload[i % sizeof(payload)]) {
			fprintf(stderr, "%s: bad payload %02x\n",
				ifname, ptr[i]);
			return 1;
		}
	}
	return 0;
}

static void loopback_send_data(char *hwaddr, int len)
{
	__u8 *ptr;
	int i;

	memcpy(frame, hwaddr, ETH_ALEN);
	memcpy(frame+6, hwaddr, ETH_ALEN);
	frame[12] = ETH_TYPE[0];
	frame[13] = ETH_TYPE[1];

	ptr = frame + ETH_HDR_SIZE;
	len -= ETH_HDR_SIZE;
	for (i = 0; i < len; i++)
		ptr[i] = payload[i % sizeof(payload)];
}

static int loopback_recv(void)
{
	int ret = 0;
	struct sockaddr_ll sll;
	socklen_t slen = sizeof(sll);
	struct packet_mreq mr;

	memset(&sll, 0, slen);
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	ret = bind(sock, (struct sockaddr *)&sll, slen);
	if (ret < 0) {
		fprintf(stderr, "%s: bind(AF_PACKET): %s\n",
			ifname, strerror(errno));
		return ret;
	}

	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	ret = setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
			 (const char *)&mr, sizeof(mr));
	if (ret < 0) {
		fprintf(stderr,
			"%s: setsockopt(PACKET_MR_PROMISC): %s\n",
			ifname, strerror(errno));
		return ret;
	}

	/* TODO: SOL_ATTACH_FILTER */
	return 0;
}

static int loopback_send(void)
{
	loopback_send_data(ifhwaddr, FRAME_SIZE);
	return 0;
}

static int do_loopback(void)
{
	struct sockaddr_ll sll;
	socklen_t slen = sizeof(sll);
	fd_set rfds;
	fd_set wfds;
	int nr_rx = 0, nr_tx = 0;
	int len;
	int ret;

	while (nr_rx < NUM_OF_PACKETS ||
	       nr_tx < NUM_OF_PACKETS) {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		if (nr_rx < NUM_OF_PACKETS)
			FD_SET(sock, &rfds);
		if (nr_tx < NUM_OF_PACKETS)
			FD_SET(sock, &wfds);
		ret = select(sock+1, &rfds, &wfds, NULL, NULL);
		if (ret < 0) {
			fprintf(stderr, "%s: select(): %s\n",
				ifname, strerror(errno));
			break;
		}
		if (ret > 0) {
			if (FD_ISSET(sock, &rfds)) {
				len = recvfrom(sock, frame, FRAME_SIZE, 0,
					       (struct sockaddr *)&sll, &slen);
				if (len < 0) {
					fprintf(stderr,
						"%s: recvfrom(): %s\n",
						ifname, strerror(errno));
					return len;
				}
				if (loopback_recv_data(len) == 0) {
					fprintf(stdout, "%s: RX %d\n",
						ifname, nr_rx);
					nr_rx++;
				}
			}
			if (FD_ISSET(sock, &wfds)) {
				memset((void *)&sll, 0, slen);
				sll.sll_family = AF_PACKET;
				sll.sll_ifindex = ifindex;
				sll.sll_halen = ETH_ALEN;
				memcpy((void *)(sll.sll_addr),
				       (void *)ifhwaddr, ETH_ALEN);

				len = sendto(sock, frame, FRAME_SIZE, 0,
					     (struct sockaddr *)&sll, slen);
				if (len < 0) {
					fprintf(stderr, "%s: sendto(): %s\n",
						ifname, strerror(errno));
					return len;
				}
				fprintf(stdout, "%s: TX %d\n",
					ifname, nr_tx);
				nr_tx++;
			}
		}
	}
	printf("%s: TX %d, RX %d frames\n", ifname, nr_tx, nr_rx);
	return 0;
}

static void sigint_handler(__unused int signum)
{
	close_sock();
}

int main(int argc, char **argv)
{
	int ret = 0;

	if (argc < 2) {
		fprintf(stderr, "\nusage %s <ifname>\n",
			argv[0]);
		return -1;
	}

	ifname = argv[1];

	signal(SIGINT, sigint_handler);

	ret = open_sock();
	if (ret < 0)
		return ret;

	ret = get_ifindex(&ifindex);
	if (ret < 0)
		return ret;
	ret = get_ifhwaddr(ifhwaddr);
	if (ret < 0)
		return ret;

	ret = loopback_recv();
	if (ret < 0)
		return ret;
	ret = loopback_send();
	if (ret < 0)
		return ret;

	ret = do_loopback();
	if (ret)
		fprintf(stderr, "%s: FAIL\n", ifname);
	else
		fprintf(stderr, "%s: PASS\n", ifname);

	close_sock();
	return 0;
}
