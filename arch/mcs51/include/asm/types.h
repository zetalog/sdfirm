#ifndef __TYPES_MCS51_H_INCLUDE__
#define __TYPES_MCS51_H_INCLUDE__

#ifdef SDCC
/* Memory Model & Storage Classes:
 *
 * Small memory model is forced to put the default variables into the data
 * section.
 * We can use __near__ to put variables into a nearer storage 'pdata'
 * section and __far__ to put variables into a farer storage 'xdata'
 * section.
 */
#define __text__		__code
#define __near__		__pdata
#define __far__			__xdata

#define __TEXT_TYPE__(__type__, __name__)	\
	typedef __text__ __type__ __name__
#endif

#endif /* __TYPES_MCS51_H_INCLUDE__ */
