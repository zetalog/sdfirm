mofcomp: $(OBJ_PATH)\$(O)\acpimon.bmf
stampinf: $(OBJ_PATH)\$(O)\acpimon.inf

$(OBJ_PATH)\$(O)\acpimon.bmf: acpimon.mof
        mofcomp -B:$@ acpimon.mof
        wmimofck $@

$(OBJ_PATH)\$(O)\acpimon.inf: acpimon.inx
	copy /Y acpimon.inx $@
	stampinf -f $@
