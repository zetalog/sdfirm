#ifndef __NET_H_INCLUDE_H__
#define __NET_H_INCLUDE_H__

#include <target/config.h>
#include <target/generic.h>

typedef uint16_t dev_mtu_t;
typedef uint32_t sk_size_t;
typedef uint8_t *sk_data_t;

#define IFNAMSIZ	6

struct netdev {
#ifdef CONFIG_NET_IFNAME
	const char ifname[IFNAMSIZ];
#endif
	dev_mtu_t mtu;
	uint8_t ifindex;
	struct list_head dev_list;
};

#ifndef WIN32
struct sockaddr {
        uint16_t sa_family;	/* address family */
        uint8_t sa_data[14];	/* up to 14 bytes of direct address */
};
#endif

struct iovec {
	sk_data_t iov_base; /* (1003.1g requires void *) */
	sk_size_t iov_len;  /* (1003.1g requires size_t) */
};

struct msghdr {
	void *msg_name;			/* Socket name			*/
	sk_size_t msg_namelen;		/* Length of name		*/
	struct iovec *msg_iov;		/* Data blocks			*/
	sk_size_t msg_iovlen;		/* Number of blocks		*/
	void *msg_control;		/* Per protocol magic (eg BSD file descriptor passing) */
	sk_size_t msg_controllen;	/* Length of cmsg list */
	unsigned msg_flags;
};

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	uint32_t	qlen;
};

/* This data is invariant across clones and lives at
 * the end of the header data, ie. at skb->end.
 */
struct skb_shared_info {
	atomic_t	dataref;
};

/* We divide dataref into two halves.  The higher 16 bits hold references
 * to the payload part of skb->data.  The lower 16 bits hold references to
 * the entire skb->data.  A clone of a headerless skb holds the length of
 * the header in skb->hdr_len.
 *
 * All users must obey the rule that the skb->data reference count must be
 * greater than or equal to the payload reference count.
 *
 * Holding a reference to the payload part means that the user does not
 * care about modifications to the header part of skb->data.
 */
#define SKB_DATAREF_SHIFT 16
#define SKB_DATAREF_MASK ((1 << SKB_DATAREF_SHIFT) - 1)

struct sk_buff {
	/* these elements must be at the beginning */
	struct sk_buff		*next;
	struct sk_buff		*prev;

	struct sock		*sk;
	struct net_device	*dev;

	sk_size_t		len,
				data_len;
	uint16_t		hdr_len;
	uint8_t			local_df:1,
				cloned:1,
				nohdr:1;

	void			(*destructor)(struct sk_buff *skb);

	/* these elements must be at the ending
	 * see alloc_skb() for details
	 */
	sk_data_t		sk_tail;
	sk_data_t		sk_end;
	sk_data_t		sk_head;
	sk_data_t		sk_data;
	sk_size_t		truesize;
	atomic_t		users;
};

struct sk_buff *alloc_skb(sk_size_t size);
void free_skb(struct sk_buff *skb);

#define skb_end_pointer(skb)		(skb->sk_end)
#define skb_tail_pointer(skb)		(skb->sk_tail)
#define skb_reset_tail_pointer(skb)	(skb->sk_tail = skb->sk_data)
#define skb_set_tail_pointer(skb, off)	(skb->sk_tail = skb->sk_data + off)

#define skb_headroom(skb)		((sk_size_t)(skb->sk_data - skb->sk_head))
#define skb_tailroom(skb)		((sk_size_t)(skb_is_nonlinear(skb) ? 0 : skb->sk_end - skb->sk_tail))

#define skb_shinfo(SKB)			((struct skb_shared_info *)(skb_end_pointer(SKB)))

#define skb_is_nonlinear(skb)		(skb->data_len)

#define skb_get(skb)			(atomic_inc(&skb->users), skb)
#define skb_hold(skb)			(atomic_inc(&skb->users))
#define skb_cloned(skb)			(skb->cloned && \
					 (atomic_read(&skb_shinfo(skb)->dataref) & SKB_DATAREF_MASK) != 1)

struct sk_buff *skb_peek_tail(struct sk_buff_head *list);

struct sk_buff *skb_dequeue(struct sk_buff_head *list);
struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list);
void skb_queue_after(struct sk_buff_head *list,
		     struct sk_buff *prev,
		     struct sk_buff *newsk);
void skb_queue_head(struct sk_buff_head *list,
		    struct sk_buff *newsk);
void skb_queue_tail(struct sk_buff_head *list,
		    struct sk_buff *newsk);
void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list);
void skb_insert(struct sk_buff *newsk,
		struct sk_buff *prev, struct sk_buff *next,
		struct sk_buff_head *list);
void skb_append(struct sk_buff *old, struct sk_buff *newsk,
		struct sk_buff_head *list);

struct sock_family {
	int family;
	const char *name;

	struct sock_proto *(*get_protocol)(int type, int protocol);
	int (*create)(struct sock *sk, int type, int protocol);
	int (*release)(struct sock *sk);
	void (*destruct)(struct sock *sk);

	void (*data_ready)(struct sock *sk);
	void (*write_space)(struct sock *sk);
	void (*error_report)(struct sock *sk);

	struct list_head link;
};

typedef enum {
	SOCK_STATE_FREE = 0,			/* not allocated		*/
	SOCK_STATE_UNCONNECTED,			/* unconnected to any socket	*/
	SOCK_STATE_CONNECTING,			/* in process of connecting	*/
	SOCK_STATE_CONNECTED,			/* connected to socket		*/
	SOCK_STATE_DISCONNECTING		/* in process of disconnecting	*/
} sock_state;

/* Sock flags */
enum sock_flags {
	SOCK_DEAD,
	SOCK_DONE,
	SOCK_ZAPPED,
	SOCK_ERROR_REPORT,
	SOCK_DATA_READY,
	SOCK_WRITE_SPACE,
	SOCK_AUTOBIND,
	SOCK_LISTENING,
	SOCK_LINGER,
	SOCK_POLLING,
};

typedef struct {
	void *owner;
} socket_lock_t;

struct sock_proto {
	const char *name;
	int (*bind)(struct sock *sock,
		    struct sockaddr *myaddr,
		    int sockaddr_len);
	int (*connect)(struct sock *sk,
		       struct sockaddr *uaddr, 
		       int addr_len);
	int (*disconnect)(struct sock *sk);
	int (*listen)(struct sock *sk, int backlog);
	int (*accept)(struct sock *sk, struct sock *newsk);
	int (*sendmsg)(struct sock *sk, struct msghdr *m, sk_size_t len);
	int (*recvmsg)(struct sock *sk, struct msghdr *m, sk_size_t len);
	int (*setsockopt)(struct sock *sk, int level,
			  int optname, const char *optval, int optlen);
	int (*getsockopt)(struct sock *sk, int level,
			  int optname, char *optval, int *optlen);
	int (*getname)(struct sock *sk,
		       struct sockaddr *addr,
		       int *sockaddr_len, int peer);
	int max_header;
	unsigned int obj_size;
	atomic_t *orphan_count;
	struct list_head link;
};

struct sock {
	struct sock_family	*sk_family;
	sock_state		sk_state;
	unsigned char		sk_reuse;
	struct hlist_node	sk_node;
	struct hlist_node	sk_bind_node;
	struct hlist_node	sk_ackq_node;
	atomic_t		sk_refcnt;
	unsigned int		sk_hash;
	int			sk_type;
	struct sock_proto	*sk_prot;
	unsigned char		sk_shutdown : 2,
				sk_no_check : 2,
				sk_userlocks : 4;
#define SOCK_SNDBUF_LOCK	1
#define SOCK_RCVBUF_LOCK	2
	unsigned char		sk_protocol;
	socket_lock_t		sk_lock;
	/*
	 * The backlog queue is special, it is always used with
	 * the per-socket spinlock held and requires low latency
	 * access. Therefore we special case it's implementation.
	 */
	struct {
		struct sk_buff *head;
		struct sk_buff *tail;
	} sk_backlog;
	struct dst_entry	*sk_dst_cache;
	atomic_t		sk_omem_alloc;

	atomic_t		sk_rmem_alloc;
	atomic_t		sk_wmem_alloc;
	int			sk_rcvbuf;
	int			sk_sndbuf;
#define SOCK_MIN_SNDBUF 2048
#define SOCK_MIN_RCVBUF 256
	struct sk_buff_head	sk_receive_queue;
	struct sk_buff_head	sk_write_queue;

	int			sk_wmem_queued;
	unsigned long 		sk_flags;
	unsigned long	        sk_lingertime;
	struct sock_proto	*sk_prot_creator;
	int			sk_err,
				sk_err_soft;
	unsigned short		sk_ack_backlog;
	unsigned short		sk_max_ack_backlog;
	struct hlist_head	sk_ack_backlog_queue;
#define SOCK_DEF_ACCEPTQ	10
	struct sock		*sk_ack_listen;
	uint32_t		sk_priority;
	void			*sk_protinfo;
	void			*sk_user_data;
#if 0
	struct mem_page		*sk_sndmsg_page;
	uint32_t		sk_sndmsg_off;
#endif
	struct sk_buff		*sk_send_head;
  	int			(*sk_backlog_rcv)(struct sock *sk,
						  struct sk_buff *skb);  
	void			(*sk_async_input)(struct sock *sk);
	void			(*sk_async_output)(struct sock *sk);
	void			(*sk_async_error)(struct sock *sk);
};

#endif /* __NET_H_INCLUDE_H__ */
