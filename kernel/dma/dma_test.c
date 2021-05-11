

phys_addr_t dmatest_rx_addr;
phys_addr_t dmatest_tx_addr;
dma_t dmatest_tx_chan;
dma_t dmatest_rx_chan;
DECLARE_CIRCBF8(dmatest_rx_buf, 32);
DECLARE_CIRCBF8(dmatest_tx_buf, 32);

void dmatest(void)
{
	dmatest_rx_chan = dma_request_channel(DMA_FROM_DEVICE, NULL);
	dmatest_tx_chan = dma_request_channel(DMA_TO_DEVICE, NULL);

	/* TODO: dma_alloc_coherent of DMA buffers */
	dmatest_rx_addr = dma_map_single(dmatest_rx_chan,
					 __pa(dmatest_rx_buf.buffer),
					 32, DMA_FROM_DEVICE);
	dmatest_tx_addr = dma_map_single(dmatest_tx_chan,
					 __pa(dmatest_tx_buf.buffer),
					 32, DMA_TO_DEVICE);
}
