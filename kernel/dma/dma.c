#include <target/dma.h>

#define dma_is_direct(dma)		true

void dma_direct_sync_single_for_cpu(dma_addr_t addr, size_t size,
				    dma_dir_t dir)
{
#if 0
	phys_addr_t paddr = dma_to_phys(dma, addr);

	if (!dma_is_coherent(dma)) {
		dma_hw_sync_cpu(paddr, size, dir);
		dma_hw_sync_cpu_all();
	}
#endif
}

dma_addr_t dma_direct_map_page(dma_t dma, struct page *page,
			       unsigned long offset, size_t size,
			       dma_dir_t dir, unsigned long attrs)
{
#if 0
	phys_addr_t phys = page_to_phys(page) + offset;
	dma_addr_t dma_addr = phys_to_dma(dma, phys);

	if (!dma_is_coherent(dma) && !(attrs & DMA_ATTR_SKIP_CPU_SYNC))
		dma_hw_sync_dev(phys, size, dir);
	return dma_addr;
#endif
	return 0;
}

void dma_direct_unmap_page(dma_t dma, dma_addr_t addr, size_t size,
			   dma_dir_t dir, unsigned long attrs)
{
#if 0
	if (!(attrs & DMA_ATTR_SKIP_CPU_SYNC))
		dma_hw_sync_cpu(dma, addr, size, dir);
#endif
}

dma_addr_t dma_map_page_attrs(dma_t dma, struct page *page,
			      size_t offset, size_t size, dma_dir_t dir,
			      unsigned long attrs)
{
	dma_addr_t addr;

	if (dma_is_direct(dma))
		addr = dma_direct_map_page(dma, page,
					   offset, size, dir, attrs);
	else
		addr = dma_hw_map_page(dma, page, offset, size, dir, attrs);
}

void dma_unmap_page_attrs(dma_t dma, dma_addr_t addr, size_t size,
			  dma_dir_t dir, unsigned long attrs)
{
	if (dma_is_direct(dma))
		dma_direct_unmap_page(dma, addr, size, dir, attrs);
	else
		dma_hw_unmap_page(dma, addr, size, dir, attrs);
}

dma_addr_t dma_map_single_attrs(dma_t dma, void *ptr, size_t size,
				dma_dir_t dir, unsigned long attrs)
{
#if 0
	return dma_map_page_attrs(dma, virt_to_page(ptr),
				  offset_in_page(ptr), size, dir, attrs);
#else
	return 0;
#endif
}

void dma_unmap_single_attrs(dma_t dma, dma_addr_t addr, size_t size,
			    dma_dir_t dir, unsigned long attrs)
{
	return dma_unmap_page_attrs(dma, addr, size, dir, attrs);
}

dma_addr_t dma_map_single(void *pages, size_t size, dma_dir_t dir)
{
	return (dma_addr_t)pages;
}

void dma_unmap_single(dma_addr_t addr, size_t size, dma_dir_t dir)
{
}

void dma_sync_single_for_dev(dma_addr_t addr, size_t size, dma_dir_t dir)
{
}

void dma_init(void)
{
	dmac_hw_ctrl_init();
}
