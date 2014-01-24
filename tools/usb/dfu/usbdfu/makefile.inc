mofcomp: $(OBJ_PATH)\$(O)\usbdfu.bmf
stampinf: $(OBJ_PATH)\$(O)\usbdfu.inf

$(OBJ_PATH)\$(O)\usbdfu.bmf: usbdfu.mof
        mofcomp -B:$@ usbdfu.mof
        wmimofck $@

$(OBJ_PATH)\$(O)\usbdfu.inf: usbdfu.inx
	copy /Y usbdfu.inx $@
	stampinf -f $@
