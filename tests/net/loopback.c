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
static char *mode = NULL;
static char *ifname = NULL;

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
		fprintf(stderr, "%s %s: socket(AF_PACKET): %s\n",
			ifname, mode, strerror(errno));
		return -EFAULT;
	}
	return 0;
}

static int get_ifhwaddr(char *ifhwaddr)
{
	struct ifreq ifr;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	if (ret < 0) {
		fprintf(stderr, "%s %s: ioctl(SIOCGIFHWADDR): %s\n",
			ifname, mode, strerror(errno));
		return ret;
	}
	memcpy(ifhwaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	return 0;
}

static int get_ifindex(int *ifindex)
{
	struct ifreq ifr;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ret = ioctl(sock, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		fprintf(stderr, "%s %s: ioctl(SIOCGIFINDEX): %s\n",
			ifname, mode, strerror(errno));
		return ret;
	}
	*ifindex = ifr.ifr_ifindex;
	return 0;
}

static int loopback_recv_data(int len)
{
	eth_hdr *hdr = (eth_hdr *)frame;
	__u8 *ptr;
	int i;

	if (len < FRAME_SIZE) {
		fprintf(stderr, "%s %s: bad packet length %d\n",
			ifname, mode, len);
		return 1;
	}
	if (hdr->type[0] != ETH_TYPE[0] || hdr->type[1] != ETH_TYPE[1]) {
		fprintf(stderr, "%s %s: bad packet type %02x%02x\n",
			ifname, mode, hdr->type[0], hdr->type[1]);
		return 1;
	}

	ptr = frame + ETH_HDR_SIZE;
	len -= ETH_HDR_SIZE;
	for (i = 0; i < len; i++) {
		if (ptr[i] != payload[i % sizeof(payload)]) {
			fprintf(stderr, "%s %s: bad payload %02x\n",
				ifname, mode, ptr[i]);
			return 1;
		}
	}
	return 0;
}

static void loopback_send_data(char *ifhwaddr, int len)
{
	__u8 *ptr;
	int i;

	memcpy(frame, ifhwaddr, ETH_ALEN);
	memcpy(frame+6, ifhwaddr, ETH_ALEN);
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
	int ifindex;
	struct sockaddr_ll sll;
	socklen_t slen = sizeof(sll);
	struct packet_mreq mr;
	int len;
	int i;

	ret = open_sock();
	if (ret < 0)
		return ret;
	ret = get_ifindex(&ifindex);
	if (ret < 0)
		return ret;

	memset(&sll, 0, slen);
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	ret = bind(sock, (struct sockaddr *)&sll, slen);
	if (ret < 0) {
		fprintf(stderr, "%s %s: bind(AF_PACKET): %s\n",
			ifname, mode, strerror(errno));
		goto err;
	}

	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	ret = setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
			 (const char *)&mr, sizeof(mr));
	if (ret < 0) {
		fprintf(stderr,
			"%s %s: setsockopt(PACKET_MR_PROMISC): %s\n",
			ifname, mode, strerror(errno));
		goto err;
	}

	/* TODO: SOL_ATTACH_FILTER */

	i = 0;
	while (i < NUM_OF_PACKETS) {
		len = recvfrom(sock, frame, FRAME_SIZE, 0,
			       (struct sockaddr *)&sll, &slen);
		if (len < 0) {
			fprintf(stderr, "%s %s, recvfrom(): %s\n",
				ifname, mode, strerror(errno));
			ret = len;
			goto err;
		}
		if (loopback_recv_data(len) == 0)
			i++;
	}
	printf("%s %s: %d frames\n", ifname, mode, i);

err:
	close_sock();
	return ret;
}

static int loopback_send(void)
{
	int ret = 0;
	struct sockaddr_ll sll;
	socklen_t slen = sizeof(sll);
	char ifhwaddr[ETH_ALEN];
	int ifindex;
	int len;
	int i;

	ret = open_sock();
	if (ret < 0)
		return ret;
	ret = get_ifhwaddr(ifhwaddr);
	if (ret < 0)
		return ret;
	ret = get_ifindex(&ifindex);
	if (ret < 0)
		return ret;

	memset((void *)&sll, 0, slen);
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_halen = ETH_ALEN;
	memcpy((void *)(sll.sll_addr), (void *)ifhwaddr, ETH_ALEN);

	loopback_send_data(ifhwaddr, FRAME_SIZE);

	i = 0;
	while (i < NUM_OF_PACKETS) {
		len = sendto(sock, frame, FRAME_SIZE, 0,
			     (struct sockaddr *)&sll, slen);
		if (len < 0) {
			fprintf(stderr, "%s %s, sendto(): %s\n",
				ifname, mode, strerror(errno));
			ret = len;
			goto err;
		}
		i++;
	}
	printf("%s %s: %d frames\n", ifname, mode, i);

err:
	close_sock();
	return ret;
}

static void sigint_handler(__unused int signum)
{
	close_sock();
}

int main(int argc, char **argv)
{
	int ret = 0;
	int i;

	if (argc < 3) {
		fprintf(stderr, "\nusage %s <ifname> <mode (rx|tx)>\n",
			argv[0]);
		return -1;
	}

	ifname = argv[1];
	mode = argv[2];

	for (i = 0; i < strlen(mode); i++)
		mode[i] = tolower(mode[i]);

	signal(SIGINT, sigint_handler);
	if (!(strcmp(mode, "rx")))
		ret = loopback_recv();
	else
		ret = loopback_send();
	if (ret)
		fprintf(stderr, "%s %s: FAIL\n", ifname, mode);
	else
		fprintf(stderr, "%s %s: PASS\n", ifname, mode);
	return 0;
}
