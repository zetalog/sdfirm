#include <target/heap.h>
#include <target/string.h>

/* The following macros are only invoked with (2n+1)-multiples of
 * heap_size_t units, with a positive integer n. This is exploited for
 * fast inline execution when n is exact.
 */
#define HEAP_ALLOC_ZERO(charp, nbytes)				\
	do {							\
		memory_set((charp), 0, (size_t)nbytes);		\
	} while (0)

#define HEAP_ALLOC_COPY(dest, src, nbytes)			\
	do {							\
		memory_copy(dest, src, (size_t)nbytes);		\
	} while (0)

/*=========================================================================
 * chunk operations
 *=======================================================================*/
/* heap_chunk details:
 *
 * Chunks of memory are maintained using a 'boundary tag' method as
 * described in e.g., Knuth or Standish.
 * See the paper by Paul Wilson for a survey of such techniques:
 * ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps
 *
 * Sizes of free chunks are stored both in the front of each chunk and at
 * the end.  This makes consolidating fragmented chunks into bigger chunks
 * very fast.  The size fields also hold bits representing whether chunks
 * are free or in use.
 *
 * An allocated chunk looks like this:
 * curr->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       | prev_size = chunk size of (prev) if P=0             | |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       | curr_size = chunk size of (curr) + HEAP_SIZE_SIZE   |P|
 *  mem->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *       | user data will begin here                             . | [inuse]
 *       .                                                       . | chunk size of (curr)
 *       .                                                       . |
 *       .                                                       | |
 * next->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *       | user data will end here                               | |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *       | curr_size = chunk size of (next) + HEAP_SIZE_SIZE   |1|
 *  mem->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       .                                                       .
 *       .                                                       .
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * NOTE: chunk size of (p) implies bytes of user data plus size of
 *       'curr_size' fields.
 *
 * Where "curr" is the front of the chunk for the purpose of most of the
 * malloc code, but "mem" is the pointer that is returned to the user.
 * "next" is the beginning of the next contiguous chunk.
 *
 * Chunks always begin on even word boundries, so the mem portion (which
 * is returned to the user) is also on an even word boundary, and thus
 * double-word aligned.
 * Free chunks are stored in circular doubly-linked lists, and look like
 * this:
 * curr->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       | prev_size = chunk size of (prev) if P=0             | |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  head:| curr_size = chunk size of (curr) + HEAP_SIZE_SIZE   |P|
 *  mem->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *       | Forward pointer to next chunk in free chunk list      | | [!inuse]
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | at least
 *       | Back pointer to previous chunk in free chunk list     | | forward/back pointers
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *       | unused space (may be 0 bytes long)                    . |
 *       .                                                       . |
 *       .                                                       | |
 * next->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *  foot:| curr_size = chunk size of (curr) + HEAP_SIZE_SIZE     |
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       | curr_size = chunk size of (next) + HEAP_SIZE_SIZE   |0|
 *  mem->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       .                                                       .
 *       .                                                       .
 *       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * NOTE: chunk size of (p) implies bytes of unused space plus size of
 *       'prev_size' and 'curr_size' fields.
 *
 * The P (HEAP_CHUNK_FLAG_P) bit, stored in the unused low-order bit of
 * the chunk size (which is always a multiple of two words), is an in-use
 * bit for the *previous* chunk.  If that bit is *clear*, then the word
 * before the current chunk size contains the previous chunk size, and can
 * be used to find the front of the previous chunk.  (The very first chunk
 * allocated always has this bit set, preventing access to non-existent
 * (or non-owned) memory.)
 *
 * Note that the 'foot' of the current chunk is actually represented as
 * the prev_size of the NEXT chunk.  (This makes it easier to deal with
 * alignments etc). 
 *
 * The two exceptions to all this are
 * 1. The special chunk 'heap_first_chunk', which doesn't bother using the
 *    trailing size field since there is no next contiguous chunk that
 *    would have to index off it.  (After initialization,
 *    'heap_first_chunk' is forced to always exist.  If it would become
 *    less than HEAP_HEAD_SIZE bytes long, it is replenished via
 *    heap_extend_top.)
 * 2. The memory mapped chunks, which are not used in this allocator
 *    implementation.
 *
 * 'av': An array of chunks serving as bin headers for consolidated
 * chunks.  Each bin is doubly linked.  The bins are approximately
 * proportionally (log) spaced.  There are a lot of these bins (128). This
 * may look excessive, but works very well in practice.  All procedures
 * maintain the invariant that no consolidated chunk physically borders
 * another one.
 * Chunks in bins are kept in size order, with ties going to the
 * approximately least recently used chunk.
 * The chunks in each bin are maintained in decreasing sorted order by
 * size.  This is irrelevant for the exact bins, which all contain the
 * same-sized chunks, but facilitates best-fit allocation for larger
 * chunks.  (These lists are just sequential.  Keeping them in order
 * almost never requires enough traversal to warrant using fancier ordered
 * data structures.)  Chunks of the same size are linked with the most
 * recently freed at the front, and allocations are taken from the back.
 * This results in LRU or FIFO allocation order, which tends to give each
 * chunk an equal opportunity to be consolidated with adjacent freed
 * chunks, resulting in larger free chunks and less fragmentation. 
 *
 * 'heap_first_chunk': The top-most available chunk (i.e., the one
 * bordering the end of available memory) is treated specially.  It is
 * never included in any bin, is used only if no other chunk is available,
 * and is released back to the system if it is very large
 * (see M_TRIM_THRESHOLD).
 *
 * 'heap_last_chunk': A bin holding only the remainder of the most
 * recently split (non-top) chunk.  This bin is checked before other
 * non-fitting chunks, so as to provide better locality for runs of
 * sequentially allocated chunks.
 */

struct heap_chunk {
	heap_size_t prev_size;
	heap_size_t curr_size;
/* Flags are held in 'curr_size' field of heap_chunk which could affect
 * the heap chunk alignment.
 */
#define HEAP_CHUNK_FLAG_P	0x1
#define HEAP_CHUNK_FLAG_ALIGN	1
#define HEAP_CHUNK_FLAG_MASK	((1<<HEAP_CHUNK_FLAG_ALIGN)-1)
	struct list_head node;
#define heap_node2chunk(p)	list_entry(p, struct heap_chunk, node)
};
#define HEAP_HEAD_SIZE		(sizeof (struct heap_chunk))

/* Requests are 'exact' if both the corresponding and the next bin are
 * exact.
 */
#define HEAP_EXACT_MAX			512
#define HEAP_EXACT_ALIGN		3
#define HEAP_EXACT_SPACE		(1<<HEAP_EXACT_ALIGN)

/* XXX: HEAP_ALIGN_SIZE Restrictions
 * HEAP_ALIGN_SIZE must >= 2^HEAP_CHUNK_FLAG_ALIGN to insure curr_size is
 * a even number to hold the HEAP_CHUNK_FLAG_XX.
 * HEAP_ALIGN_SIZE must >= 2^HEAP_EXACT_ALIGN to ensure no chunk size can
 * be smalller than the HEAP_EXACT_SPACE.
 * Thus HEAP_ALIGN_SIZE can be ensured to be an exponent of 2, which is
 * required by the ALIGN macro.
 */
#if HEAP_CHUNK_FLAG_ALIGN > HEAP_EXACT_ALIGN
#define HEAP_ALIGN_SIZE		(1<<HEAP_CHUNK_FLAG_ALIGN)
#else
#define HEAP_ALIGN_SIZE		HEAP_EXACT_SPACE
#endif
#define HEAP_ALIGN_MASK		(HEAP_ALIGN_SIZE-1)

#define heap_chunk2mem(chk)		\
	(((caddr_t)(chk) + 2*HEAP_SIZE_SIZE))
#define heap_mem2chunk(mem)		\
	((struct heap_chunk *)((caddr_t)(mem) - 2*HEAP_SIZE_SIZE))
#define heap_get_chunk_at(p, s)		\
	((struct heap_chunk *)(((caddr_t)(p)) + (s)))
#define heap_next_chunk(p)		\
	heap_get_chunk_at(p, heap_curr_size(p))
#define heap_prev_chunk(p)		\
	((struct heap_chunk *)(((caddr_t)(p)) - heap_prev_size(p)))

#define heap_offset(sz)			((heap_offset_t)((sz) & (CONFIG_HEAP_SIZE-1)))
#define heap_align(sz)			((heap_size_t)(ALIGN((sz), HEAP_ALIGN_SIZE)))

/* XXX: Pad Request Bytes into Usable Bits
 *
 * Provieded the requested new chunk is the current chunk, then the
 * current chunk will become !inuse after allocation, which means the next
 * chunk's prev_size field could be reused by the current chunk, so the
 * allocation overhead would be HEAP_SIZE_SIZE rather than
 * 2*HEAP_SIZE_SIZE.
 */
#define heap_request_pad(req)					\
	(heap_offset((req)+HEAP_SIZE_SIZE + HEAP_ALIGN_MASK) <	\
	 heap_offset(HEAP_HEAD_SIZE + HEAP_ALIGN_MASK) ?	\
	 heap_align(HEAP_HEAD_SIZE) :				\
	 heap_align((req)+HEAP_SIZE_SIZE))
#define heap_aligned_ok(m)					\
	(((caddr_t)((m)) & (HEAP_ALIGN_MASK)) == 0)

/*=========================================================================
 * size field
 *=======================================================================*/
#define heap_curr_size(p)		\
	((p)->curr_size & ~(HEAP_CHUNK_FLAG_P))
#define heap_prev_size(p)		\
	((p)->prev_size)
#define heap_set_size(p, s)		\
	((p)->curr_size = (((p)->curr_size & HEAP_CHUNK_FLAG_P) | (s)))
/* s does not include inuse bit */
#define __heap_set_head(p, s)		\
	((p)->curr_size = (s))
/* s includes inuse bit */
#define heap_set_head(p, s)		\
	((p)->curr_size = ((s)|HEAP_CHUNK_FLAG_P))
/* s does not include inuse bit */
#define heap_set_foot(p, s)		\
	(heap_get_chunk_at(p, s)->prev_size = (s))

/*=========================================================================
 * inuse field
 *=======================================================================*/
#define heap_curr_inuse(p)		\
	(heap_next_chunk(p)->curr_size & HEAP_CHUNK_FLAG_P)
#define heap_prev_inuse(p)		\
	((p)->curr_size & HEAP_CHUNK_FLAG_P)
#define heap_set_inuse(p)		\
	(heap_next_chunk(p)->curr_size |= HEAP_CHUNK_FLAG_P
#define heap_clear_inuse(p)		\
	(heap_next_chunk(p)->curr_size &= ~HEAP_CHUNK_FLAG_P)
#define heap_get_inuse_at(p, s)		\
	(heap_get_chunk_at(p, s)->curr_size & HEAP_CHUNK_FLAG_P)
#define heap_set_inuse_at(p, s)		\
	(heap_get_chunk_at(p, s)->curr_size |= HEAP_CHUNK_FLAG_P)
#define heap_clear_inuse_at(p, s)	\
	(heap_get_chunk_at(p, s)->curr_size &= ~(HEAP_CHUNK_FLAG_P))

/*=========================================================================
 * bin field
 *=======================================================================*/
/* Bins
 *
 * The bins, 'heap_bins' are an array of pairs of pointers serving as the
 * heads of (initially empty) doubly-linked lists of chunks, laid out in a
 * way so that each pair can be treated as if it were in a heap_chunk.
 * (This way, the fd/bk offsets for linking bin heads and chunks are the
 * same).
 *
 * Bins for sizes<512 bytes contain chunks of all the same size, spaced
 * 8 bytes apart.  Larger bins are approximately logarithmically spaced.
 * (See the table below.) The 'heap_bins' array is never mentioned
 * directly in the code, but instead via bin access macros.
 *
 * Bin layout:
 *
 * 64 bins of size           8 (exact)
 * 32 bins of size          64
 * 16 bins of size         512
 *  8 bins of size        4096
 *  4 bins of size       32768
 *  2 bins of size      262144
 *  1 bin  of size what's left
 *
 * There is actually a little bit of slop in the numbers in
 * heap_bin_hash for the sake of speed. This makes no difference
 * elsewhere.
 *
 * The special chunks 'heap_first_chunk' and 'heap_last_chunk' get their own
 * bins, (this is implemented via yet more trickery with the heap_bins
 * array), although 'heap_first_chunk' is never properly linked to its bin since
 * it is always handled specially.
 */

/* Indexing into bins, the macro will extract index for every size value
 * as follows:
 *
 * id                number/id            size
 * 64 bins of size           8         (0-511)
 * 32 bins of size          64      (512-2559)
 * 16 bins of size         512    (2560-10751)
 *  8 bins of size        4096   (10752-40959)
 *  4 bins of size       32768  (40960-163840)
 *  2 bins of size      262144 (163840-524288)
 *  1 bin  of size what's left
 *
 * index          size number
 * ==========================
 *     0      0-     7      8
 * ..........................
 *    63    504-   511      8 (exact)
 * ==========================
 *    64    512-   575     64
 * ..........................
 *    95   2496-  2559     64
 * ==========================
 *    96   2560-  3072    512
 * ..........................
 *   111  10239- 10751    512
 * ==========================
 *   112  10752- 12287   1536 (including < 512 blocks)
 *   113  11288- 16383   4096
 * ..........................
 *   119  36864- 40959   4096
 * ==========================
 *   120  40960- 65535  24576 (including < 4096 blocks)
 *   121  65536- 98303  32768
 * ..........................
 *   123 131072-163840  32768
 * ==========================
 *   124 163840-262143  98304 (including < 32768 blocks)
 *   125 262144-524287 262144
 * ==========================
 *   126 524288-
 */
/* TODO: Eliminate Compilation Warnings
 *
 * Though such warnings could be safely ignored, this macro would cause
 * warnings being generated by the compiler if CONFIG_HEAP_SIZE is too
 * small.
 */
#define heap_bin_hash(sz)					\
	((uint8_t)(((((heap_size_t)(sz)) >>  9) ==    0) ?	\
		    (((heap_size_t)(sz)) >>  3) :		\
		   ((((heap_size_t)(sz)) >>  9) <=    4) ?	\
		    (((heap_size_t)(sz)) >>  6) +    56 :	\
		   ((((heap_size_t)(sz)) >>  9) <=   20) ?	\
		    (((heap_size_t)(sz)) >>  9) +    91 :	\
		   ((((heap_size_t)(sz)) >>  9) <=   84) ?	\
		    (((heap_size_t)(sz)) >> 12) +   110 :	\
		   ((((heap_size_t)(sz)) >>  9) <=  340) ?	\
		    (((heap_size_t)(sz)) >> 15) +   119 :	\
		   ((((heap_size_t)(sz)) >>  9) <= 1364) ?	\
		    (((heap_size_t)(sz)) >> 18) +   124 :	\
		    126))

#if CONFIG_HEAP_SIZE < 512
#define __NR_HEAP_BINS	(CONFIG_HEAP_SIZE >> 3)
#elif CONFIG_HEAP_SIZE < (512*5)
#define __NR_HEAP_BINS	((CONFIG_HEAP_SIZE >> 6) + 56)
#elif CONFIG_HEAP_SIZE < (512*21)
#define __NR_HEAP_BINS	((CONFIG_HEAP_SIZE >> 6) + 91)
#elif CONFIG_HEAP_SIZE < (512*85)
#define __NR_HEAP_BINS	((CONFIG_HEAP_SIZE >> 12) + 110)
#elif CONFIG_HEAP_SIZE < (512*341)
#define __NR_HEAP_BINS	((CONFIG_HEAP_SIZE >> 15) + 119)
#elif CONFIG_HEAP_SIZE < (512*1365)
#define __NR_HEAP_BINS	((CONFIG_HEAP_SIZE >> 18) + 124)
#else
#define __NR_HEAP_BINS	126
#endif
#define NR_HEAP_BINS	(__NR_HEAP_BINS+1)

#define heap_bin(i)		(&(heap_bins[(i)]))
#define heap_next_bin(b)	\
	((struct list_head *)(((caddr_t)(b)) + sizeof(struct list_head)))
#define heap_prev_bin(b)	\
	((struct list_head *)(((caddr_t)(b)) - sizeof(struct list_head)))

/* Only when heap_request_pad(sz) won't return a number < 16, the first 2
 * bins will never indexed.  Such case, the corresponding heap_bins cells
 * are instead used for bookkeeping
 * This is not to save space, but to simplify indexing, maintain locality,
 * and avoid some initialization tests.
 */
#define heap_init_top	((struct heap_chunk *)INVALID_HEAP_ADDR)
struct heap_chunk *heap_last_chunk = NULL;
struct heap_chunk *heap_first_chunk = heap_init_top;

#define IAV(i)			LIST_HEAD_INIT(heap_bins[i])
static struct list_head heap_bins[NR_HEAP_BINS] = {
#if NR_HEAP_BINS > 0
        IAV(0),
#endif
#if NR_HEAP_BINS > 1
	IAV(1),
#endif
#if NR_HEAP_BINS > 2
	IAV(2),
#endif
#if NR_HEAP_BINS > 3
	IAV(3),
#endif
#if NR_HEAP_BINS > 4
	IAV(4),
#endif
#if NR_HEAP_BINS > 5
	IAV(5),
#endif
#if NR_HEAP_BINS > 6
	IAV(6),
#endif
#if NR_HEAP_BINS > 7
	IAV(7),
#endif
#if NR_HEAP_BINS > 8
	IAV(8),
#endif
#if NR_HEAP_BINS > 9
	IAV(9),
#endif
#if NR_HEAP_BINS > 10
	IAV(10),
#endif
#if NR_HEAP_BINS > 11
	IAV(11),
#endif
#if NR_HEAP_BINS > 12
	IAV(12),
#endif
#if NR_HEAP_BINS > 13
	IAV(13),
#endif
#if NR_HEAP_BINS > 14
	IAV(14),
#endif
#if NR_HEAP_BINS > 15
	IAV(15),
#endif
#if NR_HEAP_BINS > 16
	IAV(16),
#endif
#if NR_HEAP_BINS > 17
	IAV(17),
#endif
#if NR_HEAP_BINS > 18
	IAV(18),
#endif
#if NR_HEAP_BINS > 19
	IAV(19),
#endif
#if NR_HEAP_BINS > 20
	IAV(20),
#endif
#if NR_HEAP_BINS > 21
	IAV(21),
#endif
#if NR_HEAP_BINS > 22
	IAV(22),
#endif
#if NR_HEAP_BINS > 23
	IAV(23),
#endif
#if NR_HEAP_BINS > 24
	IAV(24),
#endif
#if NR_HEAP_BINS > 25
	IAV(25),
#endif
#if NR_HEAP_BINS > 26
	IAV(26),
#endif
#if NR_HEAP_BINS > 27
	IAV(27),
#endif
#if NR_HEAP_BINS > 28
	IAV(28),
#endif
#if NR_HEAP_BINS > 29
	IAV(29),
#endif
#if NR_HEAP_BINS > 30
	IAV(30),
#endif
#if NR_HEAP_BINS > 31
	IAV(31),
#endif
#if NR_HEAP_BINS > 32
	IAV(32),
#endif
#if NR_HEAP_BINS > 33
	IAV(33),
#endif
#if NR_HEAP_BINS > 34
	IAV(34),
#endif
#if NR_HEAP_BINS > 35
	IAV(35),
#endif
#if NR_HEAP_BINS > 36
	IAV(36),
#endif
#if NR_HEAP_BINS > 37
	IAV(37),
#endif
#if NR_HEAP_BINS > 38
	IAV(38),
#endif
#if NR_HEAP_BINS > 39
	IAV(39),
#endif
#if NR_HEAP_BINS > 40
	IAV(40),
#endif
#if NR_HEAP_BINS > 41
	IAV(41),
#endif
#if NR_HEAP_BINS > 42
	IAV(42),
#endif
#if NR_HEAP_BINS > 43
	IAV(43),
#endif
#if NR_HEAP_BINS > 44
	IAV(44),
#endif
#if NR_HEAP_BINS > 45
	IAV(45),
#endif
#if NR_HEAP_BINS > 46
	IAV(46),
#endif
#if NR_HEAP_BINS > 47
	IAV(47),
#endif
#if NR_HEAP_BINS > 48
	IAV(48),
#endif
#if NR_HEAP_BINS > 49
	IAV(49),
#endif
#if NR_HEAP_BINS > 50
	IAV(50),
#endif
#if NR_HEAP_BINS > 51
	IAV(51),
#endif
#if NR_HEAP_BINS > 52
	IAV(52),
#endif
#if NR_HEAP_BINS > 53
	IAV(53),
#endif
#if NR_HEAP_BINS > 54
	IAV(54),
#endif
#if NR_HEAP_BINS > 55
	IAV(55),
#endif
#if NR_HEAP_BINS > 56
        IAV(56),
#endif
#if NR_HEAP_BINS > 57
	IAV(57),
#endif
#if NR_HEAP_BINS > 58
	IAV(58),
#endif
#if NR_HEAP_BINS > 59
	IAV(59),
#endif
#if NR_HEAP_BINS > 60
	IAV(60),
#endif
#if NR_HEAP_BINS > 61
	IAV(61),
#endif
#if NR_HEAP_BINS > 62
	IAV(62),
#endif
#if NR_HEAP_BINS > 63
	IAV(63),
#endif
#if NR_HEAP_BINS > 64
	IAV(64),
#endif
#if NR_HEAP_BINS > 65
	IAV(65),
#endif
#if NR_HEAP_BINS > 66
	IAV(66),
#endif
#if NR_HEAP_BINS > 67
	IAV(67),
#endif
#if NR_HEAP_BINS > 68
	IAV(68),
#endif
#if NR_HEAP_BINS > 69
	IAV(69),
#endif
#if NR_HEAP_BINS > 70
	IAV(70),
#endif
#if NR_HEAP_BINS > 71
	IAV(71),
#endif
#if NR_HEAP_BINS > 72
	IAV(72),
#endif
#if NR_HEAP_BINS > 73
	IAV(73),
#endif
#if NR_HEAP_BINS > 74
	IAV(74),
#endif
#if NR_HEAP_BINS > 75
	IAV(75),
#endif
#if NR_HEAP_BINS > 76
	IAV(76),
#endif
#if NR_HEAP_BINS > 77
	IAV(77),
#endif
#if NR_HEAP_BINS > 78
	IAV(78),
#endif
#if NR_HEAP_BINS > 79
	IAV(79),
#endif
#if NR_HEAP_BINS > 80
	IAV(80),
#endif
#if NR_HEAP_BINS > 81
	IAV(81),
#endif
#if NR_HEAP_BINS > 82
	IAV(82),
#endif
#if NR_HEAP_BINS > 83
	IAV(83),
#endif
#if NR_HEAP_BINS > 84
	IAV(84),
#endif
#if NR_HEAP_BINS > 85
	IAV(85),
#endif
#if NR_HEAP_BINS > 86
	IAV(86),
#endif
#if NR_HEAP_BINS > 87
	IAV(87),
#endif
#if NR_HEAP_BINS > 88
	IAV(88),
#endif
#if NR_HEAP_BINS > 89
	IAV(89),
#endif
#if NR_HEAP_BINS > 90
	IAV(90),
#endif
#if NR_HEAP_BINS > 91
	IAV(91),
#endif
#if NR_HEAP_BINS > 92
	IAV(92),
#endif
#if NR_HEAP_BINS > 93
	IAV(93),
#endif
#if NR_HEAP_BINS > 94
	IAV(94),
#endif
#if NR_HEAP_BINS > 95
	IAV(95),
#endif
#if NR_HEAP_BINS > 96
	IAV(96),
#endif
#if NR_HEAP_BINS > 97
	IAV(97),
#endif
#if NR_HEAP_BINS > 98
	IAV(98),
#endif
#if NR_HEAP_BINS > 99
	IAV(99),
#endif
#if NR_HEAP_BINS > 100
	IAV(100),
#endif
#if NR_HEAP_BINS > 101
	IAV(101),
#endif
#if NR_HEAP_BINS > 102
	IAV(102),
#endif
#if NR_HEAP_BINS > 103
	IAV(103),
#endif
#if NR_HEAP_BINS > 104
	IAV(104),
#endif
#if NR_HEAP_BINS > 105
	IAV(105),
#endif
#if NR_HEAP_BINS > 106
	IAV(106),
#endif
#if NR_HEAP_BINS > 107
	IAV(107),
#endif
#if NR_HEAP_BINS > 108
	IAV(108),
#endif
#if NR_HEAP_BINS > 109
	IAV(109),
#endif
#if NR_HEAP_BINS > 110
	IAV(110),
#endif
#if NR_HEAP_BINS > 111
	IAV(111),
#endif
#if NR_HEAP_BINS > 112
        IAV(112),
#endif
#if NR_HEAP_BINS > 113
	IAV(113),
#endif
#if NR_HEAP_BINS > 114
	IAV(114),
#endif
#if NR_HEAP_BINS > 115
	IAV(115),
#endif
#if NR_HEAP_BINS > 116
	IAV(116),
#endif
#if NR_HEAP_BINS > 117
	IAV(117),
#endif
#if NR_HEAP_BINS > 118
	IAV(118),
#endif
#if NR_HEAP_BINS > 119
	IAV(119),
#endif
#if NR_HEAP_BINS > 120
	IAV(120),
#endif
#if NR_HEAP_BINS > 121
	IAV(121),
#endif
#if NR_HEAP_BINS > 122
	IAV(122),
#endif
#if NR_HEAP_BINS > 123
	IAV(123),
#endif
#if NR_HEAP_BINS > 124
	IAV(124),
#endif
#if NR_HEAP_BINS > 125
	IAV(125),
#endif
#if NR_HEAP_BINS > 126
	IAV(126),
#endif
#if NR_HEAP_BINS > 127
	IAV(127)
#endif
};

#define heap_first_node(b)	((b)->next)
#define heap_last_node(b)	((b)->prev)
#define heap_first_chunk(b)	((struct heap_chunk *)((b)->node.next))
#define heap_last_chunk(b)	((struct heap_chunk *)((b)->node.prev))

/* bins for chunks < 512 are all spaced 8 bytes apart, and hold
 * identically sized chunks while other bins are holding chunks with
 * similar sizes
 */
#define heap_is_exact_request(nb)	(nb < HEAP_EXACT_MAX-HEAP_EXACT_SPACE)
#define heap_exact_index(sz)		((uint8_t)(((heap_size_t)(sz)) >> 3))

/* To help compensate for the large number of bins, a one-level index
 * structure is used for bin-by-bin searching.  'heap_blocks' is a bitmap
 * recording whether groups of HEAP_BINS_PER_BLOCK bins have any
 * (possibly) non-empty bins, so they can be skipped over all at once
 * during traversals. The bits are NOT always cleared as soon as all bins
 * in a block are empty, but instead only when all are noticed to be empty
 * during traversal in heap_alloc.
 */
#define HEAP_BINS_PER_BLOCK	1
#define HEAP_NR_BLOCKS		\
	((NR_HEAP_BINS+HEAP_BINS_PER_BLOCK-1)/(HEAP_BINS_PER_BLOCK))

#define heap_block_index(index)	((uint8_t)(index / HEAP_BINS_PER_BLOCK))
#define heap_set_block(index)	set_bit(heap_block_index(index), heap_blocks)
#define heap_clear_block(index)	clear_bit(heap_block_index(index), heap_blocks)

DECLARE_BITMAP(heap_blocks, HEAP_NR_BLOCKS);

/*=========================================================================
 * bookkeeping data
 *=======================================================================*/
static caddr_t heap_base = INVALID_HEAP_ADDR;
static heap_size_t heap_size = 0;

#ifdef CONFIG_ALLOC_CHECK
/* These routines make a number of assertions about the states
 * of data structures that should be true at all times. If any
 * are not true, it's very likely that a user program has somehow
 * trashed memory. (It's also possible that there is a coding error
 * in malloc. In which case, please report it!)
 */
static void __heap_check_chunk(struct heap_chunk *p)
{
#define HEAP_CHUNK_BOTTOM(p)	((caddr_t)p + heap_curr_size(p))
	BUG_ON(heap_chunk2mem(p) < heap_base);
	if (p != heap_first_chunk)
		BUG_ON(HEAP_CHUNK_BOTTOM(p) > (caddr_t)heap_first_chunk);
	else
		BUG_ON(HEAP_CHUNK_BOTTOM(p) > (heap_base + heap_size));
}

static void __heap_check_free_chunk(struct heap_chunk *p)
{
	heap_size_t sz = p->curr_size & ~HEAP_CHUNK_FLAG_P;
	struct heap_chunk *next = heap_get_chunk_at(p, sz);

	__heap_check_chunk(p);

	/* Check whether it claims to be free ... */
	BUG_ON(heap_curr_inuse(p));

	/* Unless a special marker, must have OK fields */
	if (heap_curr_size(p) >= (heap_size_t)HEAP_HEAD_SIZE) {
		BUG_ON((heap_curr_size(p) & HEAP_ALIGN_MASK) != 0);
		BUG_ON(!heap_aligned_ok(heap_chunk2mem(p)));
		/* ... matching footer field */
		BUG_ON(next->prev_size != sz);
		/* ... and is fully consolidated */
		BUG_ON(!heap_prev_inuse(p));
		BUG_ON(!(next == heap_first_chunk || heap_curr_inuse(next)));

		/* ... and has minimally sane links */
		BUG_ON(heap_node2chunk(p->node.next->prev) != p);
		BUG_ON(heap_node2chunk(p->node.prev->next) != p);
	} else {
		/* markers are always of size HEAP_SIZE_SIZE */
		BUG_ON(heap_curr_size(p) != HEAP_SIZE_SIZE);
	}
}

static void __heap_check_inuse_chunk(struct heap_chunk *p)
{
	struct heap_chunk *next = heap_next_chunk(p);

	__heap_check_chunk(p);

	BUG_ON(!heap_curr_inuse(p));

	/* check pervious chunk */
	if (!heap_prev_inuse(p)) {
		struct heap_chunk *prev = heap_prev_chunk(p);
		BUG_ON(heap_next_chunk(prev) != p);
		__heap_check_free_chunk(prev);
	}
	/* check next chunk */
	if (heap_next_chunk(p) == heap_first_chunk) {
		BUG_ON(!heap_prev_inuse(next));
		BUG_ON(heap_curr_size(next) < HEAP_HEAD_SIZE);
	} else if (!heap_curr_inuse(next)) {
		__heap_check_free_chunk(next);
	}
}

static void __heap_check_malloced_chunk(struct heap_chunk *p, heap_size_t s)
{
	heap_size_t room = heap_curr_size(p) - s;
	__heap_check_inuse_chunk(p);

	BUG_ON(heap_curr_size(p) < HEAP_HEAD_SIZE);
	BUG_ON((heap_curr_size(p) & HEAP_ALIGN_MASK) != 0);
	BUG_ON(room < 0);
	BUG_ON(room >= (heap_offset_t)HEAP_HEAD_SIZE);
	BUG_ON(!heap_aligned_ok(heap_chunk2mem(p)));
	/* must be allocated at front of an available chunk */
	BUG_ON(!heap_prev_inuse(p));
}

#define heap_check_free_chunk(P)	__heap_check_free_chunk(P)
#define heap_check_inuse_chunk(P)	__heap_check_inuse_chunk(P)
#define heap_check_chunk(P)		__heap_check_chunk(P)
#define heap_check_alloc_chunk(P,N)	__heap_check_malloced_chunk(P,N)
#else
#define heap_check_free_chunk(P)
#define heap_check_inuse_chunk(P)
#define heap_check_chunk(P)
#define heap_check_alloc_chunk(P,N)
#endif

#ifdef CONFIG_ALLOC_TRIM
//#define HEAP_TRIM_THRESHOLD	(128 * 1024)
#define HEAP_TRIM_THRESHOLD	(32 * 1024)

static heap_size_t heap_ctrl_trim_threshold = HEAP_TRIM_THRESHOLD;

/* heap_trim gives memory back to the system (via negative arguments to
 * sbrk) if there is unused memory at the 'high' end of the allocation
 * pool. You can call this after freeing large blocks of memory to
 * potentially reduce the system-level memory requirements of a program.
 * However, it cannot guarantee to reduce memory.  Under some allocation
 * patterns, some large free blocks of memory will be locked between two
 * used chunks, so they cannot be given back to the system.
 *
 * The 'pad' argument to heap_trim represents the amount of free trailing
 * space to leave untrimmed. If this argument is zero, only the minimum
 * amount of memory to maintain internal data structures will be left (one
 * page or less). Non-zero arguments can be supplied to maintain enough
 * trailing space to service future expected allocations without having to
 * re-obtain memory from the system.
 *
 * heap_trim returns 1 if it actually released any memory, else 0.
 */
int heap_trim(heap_size_t pad)
{
	heap_offset_t top_size;		/* Amount of top-most memory */
	heap_offset_t extra;		/* Amount to release */
	caddr_t current_brk;		/* address returned by pre-check sbrk call */
	caddr_t new_brk;		/* address returned by negative sbrk call */
	page_size_t pagesz = PAGE_SIZE;

	top_size = heap_curr_size(heap_first_chunk);
	/* TODO: convert calculation */
	extra = ((top_size - pad - HEAP_HEAD_SIZE + (pagesz-1)) / pagesz-1) * pagesz;

	if (extra < (heap_offset_t)pagesz) {
		/* Not enough memory to release */
		return 0;
	} else {
		/* Test to make sure no one else called sbrk */
		current_brk = heap_sbrk(0);
		if (current_brk != (heap_first_chunk + top_size)) {
			return 0;/* Apparently we don't own memory; must fail */
		} else {
			new_brk = heap_sbrk((heap_offset_t)(-extra));
			if (new_brk == (caddr_t)(-1)) {
				/* failed? */
				/* Try to figure out what we have */
				current_brk = heap_sbrk(0);
				top_size = (heap_size_t)(current_brk - (caddr_t)heap_first_chunk);
				if (top_size >= (heap_offset_t)HEAP_HEAD_SIZE) {
					/* if not, we are very very dead! */
					heap_size = (heap_size_t)(current_brk - heap_base);
					heap_set_head(heap_first_chunk, top_size);
				}
				heap_check_chunk(heap_first_chunk);
				return 0;
			} else {
				/* Success. Adjust heap_first_chunk accordingly. */
				heap_set_head(heap_first_chunk, (heap_size_t)(top_size - extra));
				heap_size -= (heap_size_t)extra;
				heap_check_chunk(heap_first_chunk);
				return 1;
			}
		}
	}
}

static void heap_free_trim(heap_size_t sz)
{
	if (sz >= heap_ctrl_trim_threshold)
		heap_trim(0);
}
#else
#define heap_free_trim(sz)
#endif

#define heap_chunk_for_each_safe(P, N, B)	\
	list_for_each_entry_safe(struct heap_chunk, P, N, B, node)

#define heap_chunk_link(P, S, IDX)					\
do {									\
	struct list_head *BK, *FD;					\
	if (S < HEAP_EXACT_MAX) {					\
		IDX = heap_exact_index(S);				\
		heap_set_block(IDX);					\
		INIT_LIST_HEAD(&((P)->node));				\
		list_add_tail(&((P)->node), heap_bin(IDX));		\
	}  else {							\
		IDX = heap_bin_hash(S);					\
		BK = heap_bin(IDX);					\
		FD = BK->next;						\
		if (FD == BK) heap_set_block(IDX);			\
		else {							\
			while (FD != BK &&				\
			       S < heap_curr_size(heap_node2chunk(FD)))	\
				FD = FD->next;				\
			BK = FD->prev;					\
		}							\
		INIT_LIST_HEAD(&((P)->node));				\
		list_add_tail(&((P)->node), BK);			\
	}								\
} while (0)

#define heap_chunk_unlink(P)		list_del_init(&P->node)

#define heap_set_last_remainder(P)	(heap_last_chunk = (P))
#define heap_clear_last_remainder()	(heap_last_chunk = NULL)

static void heap_extend_top(heap_size_t nb)
{
	caddr_t brk;
	/* unusable bytes at front of sbrked space */
	heap_size_t front_misalign;
	/* bytes for 2nd sbrk call */
	heap_size_t correction;
	caddr_t new_brk;
	heap_size_t top_size;
	struct heap_chunk *old_top = heap_first_chunk;
	heap_size_t old_top_size;
	caddr_t old_end;
	/* add HEAP_HEAD_SIZE to ensure heap_first_chunk can be placed */
	heap_size_t sbrk_size = nb + HEAP_HEAD_SIZE;

	if (old_top == heap_init_top) {
		old_top_size = 0;
		old_end = 0;
		/* sbrk_size = CONFIG_HEAP_SIZE-HEAP_ALIGN_SIZE; */
	} else {
		old_top_size = heap_curr_size(old_top);
		old_end = (caddr_t)(heap_get_chunk_at(old_top,
						      old_top_size));
	}

	brk = heap_sbrk(sbrk_size);
	if (brk == INVALID_HEAP_ADDR)
		return;

	BUG_ON((brk != old_end) && (old_top != heap_init_top));
	heap_size += sbrk_size;
	if (brk == old_end) {
		/* can just add bytes to current heap_first_chunk */
		top_size = sbrk_size + old_top_size;
		heap_set_head(heap_first_chunk, top_size);
	} else {
		BUG_ON(old_top != heap_init_top);
		if (heap_base == INVALID_HEAP_ADDR) {
			heap_base = brk;
		}

		/* guarantee alignment of first new chunk made from this space */
		front_misalign = (heap_size_t)(heap_chunk2mem(brk) & HEAP_ALIGN_MASK);
		if (front_misalign == 0) {
			correction = (heap_size_t)(HEAP_ALIGN_SIZE) - front_misalign;
			brk += correction;
		} else {
			correction = 0;
		}

		/* allocate correction */
		new_brk = heap_sbrk(correction);
		if (new_brk == INVALID_HEAP_ADDR) {
			return;
		}

		heap_size += correction;

		heap_first_chunk = (struct heap_chunk *)brk;
		top_size = (heap_size_t)((new_brk+correction) - brk);
		heap_set_head(heap_first_chunk, top_size);
	}
}

/* alloc algorthim:
 *
 * The requested size is first converted into a usable form, 'nb'.
 * This currently means to add 4 bytes overhead plus possibly more to
 * obtain 8-byte alignment and/or to obtain a size of at least
 * HEAP_HEAD_SIZE (currently 16 bytes), the smallest allocatable size.
 * (All fits are considered 'exact' if they are within HEAP_HEAD_SIZE
 * bytes.)
 *
 * From there, the first successful of the following steps is taken:
 *
 * 1. The bin corresponding to the request size is scanned, and if a chunk
 *    of exactly the right size is found, it is taken.
 *
 * 2. The most recently remaindered chunk is used if it is big enough.
 *    This is a form of (roving) first fit, used only in the absence of
 *    exact fits. Runs of consecutive requests use the remainder of the
 *    chunk used for the previous such request whenever possible.  This
 *    limited use of a first-fit style allocation strategy tends to give
 *    contiguous chunks coextensive lifetimes, which improves locality and
 *    can reduce fragmentation in the long run.
 *
 * 3. Other bins are scanned in increasing size order, using a chunk big
 *    enough to fulfill the request, and splitting off any remainder.
 *    This search is strictly by best-fit; i.e., the smallest (with ties
 *    going to approximately the least recently used) chunk that fits is
 *    selected.
 *
 * 4. If large enough, the chunk bordering the end of memory ('heap_first_chunk')
 *    is split off. (This use of 'heap_first_chunk' is in accord with the best-fit
 *    search rule.  In effect, 'heap_first_chunk' is treated as larger (and thus
 *    less well fitting) than any other available chunk since it can be
 *    extended to be as large as necessary (up to system limitations).
 *
 * 5. Otherwise, the heap_first_chunk of memory is extended by obtaining more
 *    space from the system (normally using heap_sbrk).  Memory is
 *    gathered from the system (in system page-sized units) in a way that
 *    allows chunks obtained across different sbrk calls to be
 *    consolidated, but does not require contiguous memory. Thus, it
 *    should be safe to intersperse allocs with other sbrk calls.
 *
 * All allocations are made from the the 'lowest' part of any found chunk.
 * (The implementation invariant is that heap_prev_inuse is always true of
 * any allocated chunk; i.e., that each allocated chunk borders either a
 * previously allocated and still in-use chunk, or the base of its memory
 * arena.)
 */
caddr_t heap_alloc(heap_size_t bytes)
{
	struct heap_chunk *victim;
	heap_size_t victim_size;
	uint8_t idx;			/* index for bin traversal */
	struct list_head *bin;
	struct heap_chunk *remainder;
	heap_offset_t remainder_size;
	uint8_t remainder_index;
	uint8_t block;			/* index for block traversal */
	uint8_t startidx;		/* first bin of a traversed block */
	struct list_head *q;
	struct heap_chunk *n;
	heap_size_t nb;

	nb = heap_request_pad(bytes);
	BUG_ON(nb < bytes || nb > (CONFIG_HEAP_SIZE-HEAP_SIZE_SIZE));

	if (heap_is_exact_request(nb)) {
		/* faster version for exact requests */
		idx = heap_exact_index(nb);

		q = heap_bin(idx);
		if (list_empty(q)) {
			q = heap_next_bin(q);
		}
		if (!list_empty(q)) {
			victim = heap_node2chunk(heap_last_node(q));
			victim_size = heap_curr_size(victim);
			heap_chunk_unlink(victim);
			heap_set_inuse_at(victim, victim_size);
			heap_check_alloc_chunk(victim, nb);
			return heap_chunk2mem(victim);
		}

		/* set for bin scan below. We've already scanned 2 bins. */
		idx += 2;
	} else {
		idx = heap_bin_hash(nb);
		bin = heap_bin(idx);

		heap_chunk_for_each_safe(victim, n, bin) {
			victim_size = heap_curr_size(victim);
			remainder_size = victim_size - nb;

			if (remainder_size >= (heap_offset_t)HEAP_HEAD_SIZE) {
				/* too big */
				/* adjust to rescan below after checking
				 * last remainder
				 */
				--idx;
				break;
			} else if (remainder_size >= 0) {
				heap_chunk_unlink(victim);
				heap_set_inuse_at(victim, victim_size);
				heap_check_alloc_chunk(victim, nb);
				return heap_chunk2mem(victim);
			}
		}

		++idx;
	}

	/* try to use the last split-off remainder */
	victim = heap_last_chunk;
	if (victim) {
		victim_size = heap_curr_size(victim);
		remainder_size = victim_size - nb;
		if (remainder_size >= (heap_size_t)HEAP_HEAD_SIZE) {
			/* re-split */
			remainder = heap_get_chunk_at(victim, nb);
			heap_set_head(victim, nb);
			heap_set_last_remainder(remainder);
			heap_set_head(remainder, remainder_size);
			heap_set_foot(remainder, remainder_size);
			heap_check_alloc_chunk(victim, nb);
			return heap_chunk2mem(victim);
		}

		heap_clear_last_remainder();

		if (remainder_size >= 0) {
			/* exhaust */
			heap_set_inuse_at(victim, victim_size);
			heap_check_alloc_chunk(victim, nb);
			return heap_chunk2mem(victim);
		}

		/* else place in bin */
		heap_chunk_link(victim, victim_size, remainder_index);
	}

	/* if there are any possibly nonempty big-enough blocks,
	 * search for best fitting chunk by scanning bins in blockwidth units.
	 */
	block = heap_block_index(idx);
	do {
		block = find_next_set_bit(heap_blocks, HEAP_NR_BLOCKS, block);
		if (block >= HEAP_NR_BLOCKS)
			break;

		/* force to an even block boundary */
		if (block > heap_block_index(idx)) {
			idx = block*HEAP_BINS_PER_BLOCK;
		}
		while (idx < NR_HEAP_BINS) {
			startidx = idx;     /* (track incomplete blocks) */
			q = bin = heap_bin(idx);

			do {
				heap_chunk_for_each_safe(victim, n, bin) {
					victim_size = heap_curr_size(victim);
					remainder_size = victim_size - nb;

					if (remainder_size >= (heap_size_t)HEAP_HEAD_SIZE) {
						remainder = heap_get_chunk_at(victim, nb);
						heap_set_head(victim, nb);
						heap_chunk_unlink(victim);
						heap_set_last_remainder(remainder);
						heap_set_head(remainder, remainder_size);
						heap_set_foot(remainder, remainder_size);
						heap_check_alloc_chunk(victim, nb);
						return heap_chunk2mem(victim);
					} else if (remainder_size >= 0) {
						heap_set_inuse_at(victim, victim_size);
						heap_chunk_unlink(victim);
						heap_check_alloc_chunk(victim, nb);
						return heap_chunk2mem(victim);
					}
				}
				bin = heap_next_bin(bin);
				idx++;
			} while (((idx & (HEAP_BINS_PER_BLOCK - 1)) != 0) &&
				 (idx < NR_HEAP_BINS));

			/* clear out the block bit */
			do  {
				/* possibly backtrack to try to clear a
				 * partial block
				 */
				if ((startidx & (HEAP_BINS_PER_BLOCK - 1)) == 0) {
					heap_clear_block(startidx);
					break;
				}

				--startidx;
				q = heap_prev_bin(q);
			} while (heap_first_node(q) == q);
		}
		block++;
	} while (1);

	/* try to use heap_first_chunk chunk */
	/* require that there be a remainder, ensuring heap_first_chunk always exists */
	if (heap_first_chunk == heap_init_top ||
	    ((heap_offset_t)(heap_curr_size(heap_first_chunk) - nb) <
	     (heap_offset_t)HEAP_HEAD_SIZE)) {
		heap_extend_top(nb);
	}
	remainder_size = heap_curr_size(heap_first_chunk) - nb;
	if (remainder_size < (heap_offset_t)HEAP_HEAD_SIZE)
		return 0;

	victim = heap_first_chunk;
	heap_set_head(victim, nb);
	heap_first_chunk = heap_get_chunk_at(victim, nb);
	heap_set_head(heap_first_chunk, remainder_size);
	heap_check_alloc_chunk(victim, nb);
	return heap_chunk2mem(victim);
}

/* free algorithm:
 *
 * cases:
 * 1. free(0) has no effect.
 * 2. If a returned chunk borders the current high end of memory, it is
 *    consolidated into the heap_first_chunk, and if the total unused topmost memory
 *    exceeds the trim threshold, heap_trim is called.
 * 3. Other chunks are consolidated as they arrive, and placed in
 *    corresponding bins. (This includes the case of consolidating with the
 *    current 'heap_last_chunk').
 */
void heap_free(caddr_t mem)
{
	struct heap_chunk *p;
	heap_size_t hd;
	heap_size_t sz;
	uint8_t idx;
	/* next contiguous chunk */
	struct heap_chunk *next;
	heap_size_t nextsz;
	heap_size_t prevsz;
	/* track whether merging with heap_last_chunk */
	boolean islr;

	BUG_ON(mem == 0);
	BUG_ON(mem < heap_base);

	p = heap_mem2chunk(mem);
	hd = p->curr_size;

	heap_check_inuse_chunk(p);

	sz = hd & ~HEAP_CHUNK_FLAG_P;
	next = heap_get_chunk_at(p, sz);
	nextsz = heap_curr_size(next);

	if (next == heap_first_chunk) {
		/* merge with heap_first_chunk */
		sz += nextsz;

		if (!(hd & HEAP_CHUNK_FLAG_P)) {
			/* consolidate backward */
			prevsz = p->prev_size;
			p = heap_get_chunk_at(p, -((heap_offset_t)prevsz));
			sz += prevsz;
			heap_chunk_unlink(p);
		}

		heap_set_head(p, sz);
		heap_first_chunk = p;
		heap_free_trim(sz);
		return;
	}

	/* clear inuse bit */
	__heap_set_head(next, nextsz);

	islr = false;

	if (!(hd & HEAP_CHUNK_FLAG_P))  {
		/* consolidate backward */
		prevsz = p->prev_size;
		p = heap_get_chunk_at(p, -((heap_offset_t)prevsz));
		sz += prevsz;

		/* keep as heap_last_chunk */
		if (heap_first_chunk(p) == heap_last_chunk)
			islr = true;
		else
			heap_chunk_unlink(p);
	}

	if (!(heap_get_inuse_at(next, nextsz))) {
		sz += nextsz;
		if (!islr && heap_first_chunk(next) == heap_last_chunk) {
			islr = true;
			heap_set_last_remainder(p);
		} else {
			heap_chunk_unlink(next);
		}
	}

	heap_set_head(p, sz);
	heap_set_foot(p, sz);
	if (!islr)
		heap_chunk_link(p, sz, idx);
}

#ifdef CONFIG_ALLOC_REALLOC
/* realloc algorithm:
 *
 * If the reallocation is for additional space, and the chunk can be
 * extended, it is, else a malloc-copy-free sequence is taken.  There are
 * several different ways that a chunk could be extended. All are tried:
 *
 * Extending forward into following adjacent free chunk.
 * Shifting backwards, joining preceding adjacent space
 * Both shifting backwards and extending forward.
 * Extending into newly sbrked space
 *
 * realloc with a size argument of zero (re)allocates a minimum-sized
 * chunk.
 *
 * If the reallocation is for less space, and the new request is for a
 * 'exact' (<512 bytes) size, then the newly unused space is lopped off
 * and freed.
 * 
 * The old unix realloc convention of allowing the last-free'd chunk
 * to be used as an argument to realloc is no longer supported.
 * I don't know of any programs still relying on this feature, and
 * allowing it would also allow too many other incorrect usages of realloc
 * to be sensible.
 */
caddr_t heap_realloc(caddr_t oldmem, heap_size_t bytes)
{
	heap_size_t nb;
	struct heap_chunk *oldp;
	heap_size_t oldsize;
	struct heap_chunk *newp;
	heap_size_t newsize;
	caddr_t newmem;
	/* next contiguous chunk after oldp */
	struct heap_chunk *next;
	heap_size_t nextsize;
	/* previous contiguous chunk before oldp */
	struct heap_chunk *prev;
	heap_size_t prevsize;
	struct heap_chunk *remainder;
	heap_offset_t remainder_size;

	/* realloc of null is supposed to be same as malloc */
	if (oldmem == 0) return heap_alloc(bytes);

	newp = oldp = heap_mem2chunk(oldmem);
	newsize = oldsize = heap_curr_size(oldp);

	nb = heap_request_pad(bytes);
	BUG_ON(nb < bytes || nb > (CONFIG_HEAP_SIZE-HEAP_HEAD_SIZE));

	heap_check_inuse_chunk(oldp);

	if (oldsize < nb) {
		/* Try expanding forward */
		next = heap_get_chunk_at(oldp, oldsize);
		if (next == heap_first_chunk || !heap_curr_inuse(next)) {
			nextsize = heap_curr_size(next);
			/* Forward into heap_first_chunk only if a remainder */
			if (next == heap_first_chunk) {
				if ((heap_offset_t)(nextsize + newsize) >=
				    (heap_offset_t)(nb + HEAP_HEAD_SIZE)) {
					newsize += nextsize;
					heap_first_chunk = heap_get_chunk_at(oldp, nb);
					heap_set_head(heap_first_chunk, (newsize - nb));
					heap_set_size(oldp, nb);
					return heap_chunk2mem(oldp);
				}
			} else if ((nextsize + newsize) >= nb) {
				/* Forward into next chunk */
				heap_chunk_unlink(next);
				newsize += nextsize;
				goto split;
			}
		} else {
			next = 0;
			nextsize = 0;
		}

		/* Try shifting backwards. */
		if (!heap_prev_inuse(oldp)) {
			prev = heap_prev_chunk(oldp);
			prevsize = heap_curr_size(prev);

			/* Try forward + backward first to save a later
			 * consolidation
			 */
			if (next != 0) {
				/* into heap_first_chunk */
				if (next == heap_first_chunk) {
					if ((heap_offset_t)(nextsize + prevsize + newsize) >=
					    (heap_offset_t)(nb + HEAP_HEAD_SIZE)) {
						heap_chunk_unlink(prev);
						newp = prev;
						newsize += prevsize + nextsize;
						newmem = heap_chunk2mem(newp);
						HEAP_ALLOC_COPY(newmem, oldmem, oldsize - HEAP_SIZE_SIZE);
						heap_first_chunk = heap_get_chunk_at(newp, nb);
						heap_set_head(heap_first_chunk, (newsize - nb));
						heap_set_size(newp, nb);
						return newmem;
					}
				} else if ((nextsize + prevsize + newsize) >= nb) {
					/* into next chunk */
					heap_chunk_unlink(next);
					heap_chunk_unlink(prev);
					newp = prev;
					newsize += nextsize + prevsize;
					newmem = heap_chunk2mem(newp);
					HEAP_ALLOC_COPY(newmem, oldmem, oldsize - HEAP_SIZE_SIZE);
					goto split;
				}
			}

			/* backward only */
			if (prev != 0 &&
			    (prevsize + newsize) >= nb) {
				heap_chunk_unlink(prev);
				newp = prev;
				newsize += prevsize;
				newmem = heap_chunk2mem(newp);
				HEAP_ALLOC_COPY(newmem, oldmem, oldsize - HEAP_SIZE_SIZE);
				goto split;
			}
		}

		/* Must allocate */
		newmem = heap_alloc(bytes);
		if (newmem == 0)
			return 0;

		/* Avoid copy if newp is next chunk after oldp. */
		/* (This can only happen when new chunk is sbrk'ed.) */
		if ((newp = heap_mem2chunk(newmem)) == heap_next_chunk(oldp)) {
			newsize += heap_curr_size(newp);
			newp = oldp;
			goto split;
		}

		HEAP_ALLOC_COPY(newmem, oldmem, oldsize - HEAP_SIZE_SIZE);
		heap_free(oldmem);
		return newmem;
	}

split:
	/* split off extra room in old or expanded chunk */
	if ((heap_offset_t)(newsize - nb) >=
	    (heap_offset_t)HEAP_HEAD_SIZE) {
		/* split off remainder */
		remainder = heap_get_chunk_at(newp, nb);
		remainder_size = newsize - nb;
		heap_set_size(newp, nb);
		heap_set_head(remainder, remainder_size);
		heap_set_inuse_at(remainder, remainder_size);
		/* let free() deal with it */
		heap_free(heap_chunk2mem(remainder));
	} else {
		heap_set_size(newp, newsize);
		heap_set_inuse_at(newp, newsize);
	}

	heap_check_inuse_chunk(newp);
	return heap_chunk2mem(newp);
}
#endif

caddr_t heap_calloc(heap_size_t bytes)
{
	struct heap_chunk *p;
	heap_size_t csz;
	caddr_t mem = heap_alloc(bytes);

	if (mem == 0) {
		return 0;
	} else {
		p = heap_mem2chunk(mem);
		csz = heap_curr_size(p);
		HEAP_ALLOC_ZERO((caddr_t)mem, csz - HEAP_SIZE_SIZE);
		return mem;
	}
}

void heap_alloc_init(void)
{
}
