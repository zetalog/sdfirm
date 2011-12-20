      cmd_sdfirm = $(LD) $(LDFLAGS) $(LDFLAGS_sdfirm) -o $@ \
	-T $(sdfirm-lds) $(sdfirm-init)                        \
	--start-group $(sdfirm-main) --end-group               \
	$(filter-out $(sdfirm-lds) $(sdfirm-init) $(sdfirm-main) FORCE ,$^)
