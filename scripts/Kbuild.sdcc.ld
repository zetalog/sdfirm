      cmd_sdfirm =                                        \
	$(foreach f,                                      \
	  $(filter %.lib, $(sdfirm-init) $(sdfirm-main)), \
	  $(RANLIB) $(f);)                                \
	$(LD) $(LDFLAGS) $(LDFLAGS_sdfirm) -o $@          \
	 $(sdfirm-init) $(sdfirm-main)                    \
	 $(filter-out $(sdfirm-init) $(sdfirm-main) $(sdfirm-lds) FORCE, $^)
