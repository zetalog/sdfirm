#ifndef __RPMI_MBOX_H_INCLUDE__
#define __RPMI_MBOX_H_INCLUDE__

#include <target/rpmi.h>
#include <target/sbi.h>

#define rpmi_u32_count(__var)	(sizeof(__var) / sizeof(uint32_t))

/** Typical RPMI normal request with at least status code in response */
int rpmi_normal_request_with_status(struct mbox_chan *chan, uint32_t service_id,
				    void *req, uint32_t req_words,
				    uint32_t req_endian_words,
				    void *resp, uint32_t resp_words,
				    uint32_t resp_endian_words);

/* RPMI posted request which is without any response*/
int rpmi_posted_request(struct mbox_chan *chan, uint32_t service_id,
			void *req, uint32_t req_words, uint32_t req_endian_words);

#endif /* __RPMI_MBOX_H_INCLUDE__ */
