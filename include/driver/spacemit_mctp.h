#ifndef __MCTP_SPACEMIT_H_INCLUDE__
#define __MCTP_SPACEMIT_H_INCLUDE__

#define MCTPE_REG(offset)			(MCTPE_BASE + (offset))

#define MCTPE_Global_CTRL			MCTPE_REG(0x000)
#define MCTPE_RX_CTRL				MCTPE_REG(0x100)
#define MCTPE_RX_IRQ_EN				MCTPE_REG(0x104)
#define MCTPE_RX_Status				MCTPE_REG(0x108)
#define MCTPE_RX_ERR_Cause			MCTPE_REG(0x10C)
#define MCTPE_RX_FIFO_Status			MCTPE_REG(0x110)
#define MCTPE_RX_Descriptor_Base_Addr_Low	MCTPE_REG(0x114)
#define MCTPE_RX_Descriptor_Base_Addr_High	MCTPE_REG(0x118)
#define MCTPE_RX_Descriptor_DMA_Read_Pointer	MCTPE_REG(0x11C)
#define MCTPE_RX_Descriptor_SFT_Write_Pointer	MCTPE_REG(0x120)
#define MCTPE_RX_Descriptor_Ring_Buffer_Size	MCTPE_REG(0x124)
#define MCTPE_RX_Descriptor_Pending_Watermark	MCTPE_REG(0x128)
#define MCTPE_RX_CTX_CTRL			MCTPE_REG(0x12C)
#define MCTPE_RX_CTX_Transfer_Size		MCTPE_REG(0x130)
#define MCTPE_RX_CTX_Buffer_Address_Low		MCTPE_REG(0x134)
#define MCTPE_RX_CTX_Buffer_Address_High	MCTPE_REG(0x138)
#define MCTPE_TX_CTRL				MCTPE_REG(0x200)
#define MCTPE_TX_IRQ_EN				MCTPE_REG(0x204)
#define MCTPE_TX_Status				MCTPE_REG(0x208)
#define MCTPE_TX_ERR_Cause			MCTPE_REG(0x20C)
#define MCTPE_TX_FIFO_Status			MCTPE_REG(0x210)
#define MCTPE_TX_Descriptor_Base_Addr_Low	MCTPE_REG(0x214)
#define MCTPE_TX_Descriptor_Base_Addr_High	MCTPE_REG(0x218)
#define MCTPE_TX_Descriptor_DMA_Read_Pointer	MCTPE_REG(0x21C)
#define MCTPE_TX_Descriptor_SFT_Write_Pointer	MCTPE_REG(0x220)
#define MCTPE_TX_Descriptor_Ring_Buffer_Size	MCTPE_REG(0x224)
#define MCTPE_TX_Descriptor_Pending_Watermark	MCTPE_REG(0x228)
#define MCTPE_TX_CTX_CTRL			MCTPE_REG(0x22C)
#define MCTPE_TX_CTX_Transfer_Size		MCTPE_REG(0x230)
#define MCTPE_TX_CTX_Buffer_Address_Low		MCTPE_REG(0x234)
#define MCTPE_TX_CTX_Buffer_Address_High	MCTPE_REG(0x238)

/* MCTPE_Global_CTRL */
#define MCTPE_EN				_BV(0)
#define MCTPE_MPS_OFFSET			1
#define MCTPE_MPS_MASK				REG_2BIT_MASK
#define MCTPE_MPS(value)			_SET_FV(MCTPE_MPS, ((value) >> 6) - 1)

/* MCTPE_RX_CTRL */
#define MCTPE_RX_Pause				_BV(0)

/* MCTPE_RX_IRQ_EN
 * MCTPE_RX_Status
 * MCTPE_TX_IRQ_EN
 * MCTPE_TX_Status
 */
#define MCTPE_IRQ_Current_CTX_Done		_BV(0)
#define MCTPE_IRQ_Stop				_BV(1)
#define MCTPE_IRQ_Meet_Watermark		_BV(2)
#define MCTPE_IRQ_All_Performed			_BV(3)
#define MCTPE_IRQ_Bus_ERR			_BV(4)
#define MCTPE_IRQ_Size_ERR			_BV(5)
#define MCTPE_IRQ_SFT_ERR			_BV(6)

/* MCTPE_RX_ERR_Cause
 * MCTPE_TX_ERR_Cause
 */
#define MCTPE_Bus_ERR_Cause_OFFSET		0
#define MCTPE_Bus_ERR_Cause_MASK		REG_2BIT_MASK
#define MCTPE_Bus_ERR_Cause(value)		_GET_FV(MCTPE_Bus_ERR_Cause, value)
#define MCTPE_Size_ERR_Cause_OFFSET		2
#define MCTPE_Size_ERR_Cause_MASK		REG_3BIT_MASK
#define MCTPE_Size_ERR_Cause(value)		_GET_FV(MCTPE_Size_ERR_Cause, value)
#define MCTPE_SFT_ERR_Cause_OFFSET		5
#define MCTPE_SFT_ERR_Cause_MASK		REG_3BIT_MASK
#define MCTPE_SFT_ERR_Cause(value)		_GET_FV(MCTPE_SFT_ERR_Cause, value)

/* MCTPE_RX_FIFO_Status
 * MCTPE_TX_FIFO_Status
 */
#define MCTPE_FIFO_Depth_OFFSET			0
#define MCTPE_FIFO_Depth_MASK			REG_6BIT_MASK
#define MCTPE_FIFO_Depth(value)			_GET_FV(MCTPE_FIFO_Depth, value)
#define MCTPE_FIFO_Empty			_BV(6)
#define MCTPE_FIFO_Full				_BV(7)

/* MCTPE_RX_Descriptor_DMA_Read_Pointer
 * MCTPE_TX_Descriptor_DMA_Read_Pointer
 */
#define MCTPE_DMA_Read_Index_OFFSET		0
#define MCTPE_DMA_Read_Index_MASK		REG_13BIT_MASK
#define MCTPE_DMA_Read_Index(value)		_SET_FV(MCTPE_DMA_Read_Index, value)
#define MCTPE_DMA_Read_Index_CB			_BV(31)

/* MCTPE_RX_Descriptor_SFT_Write_Pointer
 * MCTPE_TX_Descriptor_SFT_Write_Pointer
 */
#define MCTPE_SFT_Write_Index_OFFSET		0
#define MCTPE_SFT_Write_Index_MASK		REG_13BIT_MASK
#define MCTPE_SFT_Write_Index(value)		_SET_FV(MCTPE_SFT_Write_Index, value)
#define MCTPE_SFT_Write_Index_CB		_BV(31)

/* MCTPE_RX_CTX_CTRL
 * MCTPE_TX_CTX_CTRL
 */
#define MCTPE_Done_Flag				_BV(0)
#define MCTPE_Stop				_BV(1)

/* NCTPE_TX_CTRL */
#define MCTPE_TX_Start				_BV(0)

#endif /* __MCTP_SPACEMIT_H_INCLUDE__ */
