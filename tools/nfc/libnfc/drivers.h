/*-
 * Public platform independent Near Field Communication (NFC) library
 * 
 * Copyright (C) 2009, Roel Verdult
 * Copyright (C) 2011, Romuald Conty, Romain Tartière
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __NFC_DRIVERS_H_INCLUDE__
#define __NFC_DRIVERS_H_INCLUDE__

#include <host/nfc-types.h>

#include "drivers/acr122.h"
#include "drivers/pn532_uart.h"
#include "drivers/pn53x_usb.h"

#ifdef DRIVER_ARYGON_ENABLED
#include "drivers/arygon.h"
#endif

#define DRIVERS_MAX_DEVICES         16

extern const struct nfc_driver_t *nfc_drivers[];

#endif /* __NFC_DRIVERS_H_INCLUDE__ */
