/*
 *  init/version.c
 *
 *  Copyright (C) 1992  Theodore Ts'o
 */

#include <target/version.h>

#define version(a) Version_ ## a
#define version_string(a) version(a)

int version_string(SDFIRM_VERSION_CODE);

/* FIXED STRINGS! Don't touch! */
const char sdfirm_banner[] =
	"Small Device Firmware version " UTS_RELEASE " (" SDFIRM_COMPILE_BY "@"
	SDFIRM_COMPILE_HOST ") (" SDFIRM_COMPILER ") " UTS_VERSION "\n";

