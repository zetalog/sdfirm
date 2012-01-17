/*-
 * Public platform independent Near Field Communication (NFC) library
 * 
 * Copyright (C) 2009, Roel Verdult, Romuald Conty
 * Copyright (C) 2010, Roel Verdult, Romuald Conty, Romain Tartière
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <host/nfc.h>
#include "nfc-internal.h"
#include "drivers.h"

#define LOG_CATEGORY	"libnfc.general"

const struct nfc_driver_t *nfc_drivers[] = {
	&acr122_driver,
	&pn532_uart_driver,
	&pn53x_usb_driver,
#ifdef DRIVER_ARYGON_ENABLED
	&arygon_driver,
#endif
	NULL
};

/* Connect to a NFC device
 * pndd - device description if specific device is wanted, NULL otherwise
 * Returns pointer to a nfc_device_t struct if successfull;
 *         otherwise returns NULL value.
 * If pndd is NULL, the first available NFC device is claimed.
 * It will automatically search the system using all available drivers to
 * determine a device is NFC-enabled.
 * If pndd is passed then this function will try to claim the right device
 * using information provided by the nfc_device_desc_t struct.
 * When it has successfully claimed a NFC device, memory is allocated to
 * save the device information. It will return a pointer to a nfc_device_t
 * struct.
 * This pointer should be supplied by every next functions of libnfc that
 * should perform an action with this device.
 * NOTE Depending on the desired operation mode, the device needs to be
 *      configured by using nfc_initiator_init() or nfc_target_init(),
 *      optionally followed by manual tuning of the parameters if the
 *      default parameters are not suiting your goals.
 */
nfc_device_t *nfc_connect(nfc_device_desc_t *pndd)
{
	nfc_device_t *pnd = NULL;
	const struct nfc_driver_t *ndr;
	const struct nfc_driver_t **pndr;
	
	if (pndd == NULL) {
		size_t szDeviceFound;
		nfc_device_desc_t ndd[1];
		nfc_list_devices(ndd, 1, &szDeviceFound);
		if (szDeviceFound) {
			pndd = &ndd[0];
		}
	}
	
	if (pndd == NULL)
		return NULL;
	
	log_init();
	/* Search through the device list for an available device */
	pndr = nfc_drivers;
	while ((ndr = *pndr)) {
		/* Specific device is requested: using device description
		 * pndd
		 */
		if (0 != strcmp(ndr->name, pndd->pcDriver)) {
			pndr++;
			continue;
		} else {
			pnd = ndr->connect(pndd);
		}
		
		/* Test if the connection was successful */
		if (pnd != NULL) {
			log_put(LOG_CATEGORY, NFC_PRIORITY_TRACE,
				"[%s] has been claimed.", pnd->acName);
			return pnd;
		} else {
			log_put(LOG_CATEGORY, NFC_PRIORITY_TRACE,
				"No device found using driver: %s", ndr->name);
		}
		pndr++;
	}
	log_fini();
	return NULL;
}

/* Disconnect from a NFC device
 * pnd - nfc_device_t struct pointer that represent currently used device
 * Initiator's selected tag is disconnected and the device, including
 * allocated nfc_device_t struct, is released.
 */
void nfc_disconnect(nfc_device_t *pnd)
{
	if (pnd) {
		/* Go in idle mode */
		nfc_idle(pnd);
		/* Disconnect, clean up and release the device */
		pnd->driver->disconnect(pnd);
		log_fini();
	}
}

/* Probe for discoverable supported devices (ie. only available for some
 * drivers)
 * [out]pnddDevices - array of nfc_device_desc_t previously allocated by
 *                    the caller.
 * szDevices - size of the pnddDevices array.
 * [out]pszDeviceFound - number of devices found.
 */
void nfc_list_devices(nfc_device_desc_t pnddDevices[],
		      size_t szDevices, size_t *pszDeviceFound)
{
	size_t szN;
	const struct nfc_driver_t *ndr;
	const struct nfc_driver_t **pndr = nfc_drivers;
	*pszDeviceFound = 0;
	
	log_init();
	while ((ndr = *pndr)) {
		szN = 0;
		if (ndr->probe(pnddDevices + (*pszDeviceFound),
			       szDevices - (*pszDeviceFound), &szN)) {
			*pszDeviceFound += szN;
			log_put(LOG_CATEGORY, NFC_PRIORITY_TRACE,
				"%ld device(s) found using %s driver",
				(unsigned long)szN, ndr->name);
			if (*pszDeviceFound == szDevices)
				break;
		}
		pndr++;
	}
	log_fini();
}

/* Configure advanced NFC device settings
 * pnd - nfc_device_t struct pointer that represent currently used device
 * ndo - nfc_device_option_t struct that contains option to set to device
 * bEnable - boolean to activate/disactivate the option
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * Configures parameters and registers that control for example timing,
 * modulation, frame and error handling.  There are different categories
 * for configuring thePN53X chip features (handle, activate, infinite and
 * accept).
 */
bool nfc_configure(nfc_device_t *pnd, const nfc_device_option_t ndo,
		   const bool bEnable)
{
	if (pnd->driver->configure) {
		return pnd->driver->configure(pnd, ndo, bEnable);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Initialize NFC device as initiator (reader)
 * pnd - nfc_device_t struct pointer that represent currently used device
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * The NFC device is configured to function as RFID reader.
 * After initialization it can be used to communicate to passive RFID tags
 * and active NFC devices.
 * The reader will act as initiator to communicate peer 2 peer (NFCIP) to
 * other active NFC devices.
 * - Crc is handled by the device (NDO_HANDLE_CRC = true)
 * - Parity is handled the device (NDO_HANDLE_PARITY = true)
 * - Cryto1 cipher is disabled (NDO_ACTIVATE_CRYPTO1 = false)
 * - Easy framing is enabled (NDO_EASY_FRAMING = true)
 * - Auto-switching in ISO14443-4 mode is enabled (NDO_AUTO_ISO14443_4 = true)
 * - Invalid frames are not accepted (NDO_ACCEPT_INVALID_FRAMES = false)
 * - Multiple frames are not accepted (NDO_ACCEPT_MULTIPLE_FRAMES = false)
 * - 14443-A mode is activated (NDO_FORCE_ISO14443_A = true)
 * - speed is set to 106 kbps (NDO_FORCE_SPEED_106 = true)
 * - Let the device try forever to find a target (NDO_INFINITE_SELECT = true)
 * - RF field is shortly dropped (if it was enabled) then activated again
 */
bool nfc_initiator_init(nfc_device_t *pnd)
{
	/* Drop the field for a while */
	if (!nfc_configure (pnd, NDO_ACTIVATE_FIELD, false))
		return false;
	/* Enable field so more power consuming cards can power themselves
	 * up
	 */
	if (!nfc_configure (pnd, NDO_ACTIVATE_FIELD, true))
		return false;
	/* Let the device try forever to find a target/tag */
	if (!nfc_configure (pnd, NDO_INFINITE_SELECT, true))
		return false;
	/* Activate auto ISO14443-4 switching by default */
	if (!nfc_configure (pnd, NDO_AUTO_ISO14443_4, true))
		return false;
	/* Force 14443-A mode */
	if (!nfc_configure (pnd, NDO_FORCE_ISO14443_A, true))
		return false;
	/* Force speed at 106kbps */
	if (!nfc_configure (pnd, NDO_FORCE_SPEED_106, true))
		return false;
	/* Disallow invalid frame */
	if (!nfc_configure (pnd, NDO_ACCEPT_INVALID_FRAMES, false))
		return false;
	/* Disallow multiple frames */
	if (!nfc_configure (pnd, NDO_ACCEPT_MULTIPLE_FRAMES, false))
		return false;
	/* Make sure we reset the CRC and parity to chip handling. */
	if (!nfc_configure (pnd, NDO_HANDLE_CRC, true))
		return false;
	if (!nfc_configure (pnd, NDO_HANDLE_PARITY, true))
		return false;
	/* Activate "easy framing" feature by default */
	if (!nfc_configure (pnd, NDO_EASY_FRAMING, true))
		return false;
	/* Deactivate the CRYPTO1 cipher, it may could cause problems when
	 * still active
	 */
	if (!nfc_configure (pnd, NDO_ACTIVATE_CRYPTO1, false))
		return false;
	if (pnd->driver->initiator_init) {
		return pnd->driver->initiator_init(pnd);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Select a passive or emulated tag
 * [out]pnt - nfc_target_t struct pointer which will filled if available
 * pnd - nfc_device_t struct pointer that represent currently used device
 * nm - desired modulation
 * pbtInitData - optional initiator data used for Felica, ISO14443B, Topaz
 *               polling or to select a specific UID in ISO14443A.
 * szInitData - length of initiator data pbtInitData.
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * NOTE pbtInitData is used with different kind of data depending on
 *      modulation type:
 * - for an ISO/IEC 14443 type A modulation, pbbInitData contains the UID
 *   you want to select;
 * - for an ISO/IEC 14443 type B modulation, pbbInitData contains
 *   Application Family Identifier (AFI) (see ISO/IEC 14443-3);
 * - for a FeliCa modulation, pbbInitData contains polling payload (see
 *   ISO/IEC 18092 11.2.2.5).
 * The NFC device will try to find one available passive tag or emulated tag. 
 * The chip needs to know with what kind of tag it is dealing with, therefore
 * the initial modulation and speed (106, 212 or 424 kbps) should be supplied.
 */
bool nfc_initiator_select_passive_target(nfc_device_t *pnd,
					 const nfc_modulation_t nm,
					 const byte_t *pbtInitData,
					 const size_t szInitData,
					 nfc_target_t *pnt)
{
	byte_t  *abtInit;
	bool res = false;
	size_t  szInit;
	
	abtInit = malloc(MAX(12, szInitData));
	if (!abtInit) return false;
	switch (nm.nmt) {
	case NMT_ISO14443A:
		iso14443_cascade_uid(pbtInitData, szInitData, abtInit, &szInit);
		break;
	default:
		memcpy(abtInit, pbtInitData, szInitData);
		szInit = szInitData;
		break;
	}
	if (pnd->driver->initiator_select_passive_target) {
		res = pnd->driver->initiator_select_passive_target(pnd, nm,
								   abtInit,
								   szInit,
								   pnt);
	} else {
		pnd->iLastError = EDEVNOTSUP;
	}
	
	free(abtInit);
	return res;
}

/* List passive or emulated tags
 * pnd - nfc_device_t struct pointer that represent currently used device
 * nm - desired modulation
 * [out]ant - array of nfc_target_t that will be filled with targets info
 * szTargets - size of ant (will be the max targets listed)
 * [out]pszTargetFound - pointer where target found counter will be stored
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * The NFC device will try to find the available passive tags. Some NFC
 * devices are capable to emulate passive tags. The standards (ISO18092
 * and ECMA-340) describe the modulation that can be used for reader to
 * passive communications. The chip needs to know with what kind of tag it
 * is dealing with, therefore the initial modulation and speed (106, 212
 * or 424 kbps) should be supplied.
 */
bool nfc_initiator_list_passive_targets(nfc_device_t *pnd,
					const nfc_modulation_t nm,
					nfc_target_t ant[],
					const size_t szTargets,
					size_t *pszTargetFound)
{
	nfc_target_t nt;
	size_t szTargetFound = 0;
	byte_t *pbtInitData = NULL;
	size_t szInitDataLen = 0;
	size_t i;
	bool seen = false;
	
	pnd->iLastError = 0;
	
	/* Let the reader only try once to find a tag */
	if (!nfc_configure (pnd, NDO_INFINITE_SELECT, false)) {
		return false;
	}
	
	prepare_initiator_data(nm, &pbtInitData, &szInitDataLen);
	
	while (nfc_initiator_select_passive_target(pnd, nm, pbtInitData,
						   szInitDataLen, &nt)) {
		nfc_initiator_deselect_target (pnd);
		if (szTargets == szTargetFound) {
			break;
		}
		seen = false;
		/* Check if we've already seen this tag */
		for (i = 0; i < szTargetFound; i++) {
			if (memcmp(&(ant[i]), &nt, sizeof (nfc_target_t)) == 0) {
				seen = true;
			}
		}
		if (seen) {
			break;
		}
		memcpy(&(ant[szTargetFound]), &nt, sizeof (nfc_target_t));
		szTargetFound++;
		/* deselect has no effect on FeliCa and Jewel cards so
		 * we'll stop after one...
		 */
		/* ISO/IEC 14443 B' cards are polled at 100% probability
		 * so it's not possible to detect correctly two cards at
		 * the same time
		 */
		if ((nm.nmt == NMT_FELICA) ||
		    (nm.nmt == NMT_JEWEL) ||
		    (nm.nmt == NMT_ISO14443BI) ||
		    (nm.nmt == NMT_ISO14443B2SR) ||
		    (nm.nmt == NMT_ISO14443B2CT)) {
			break;
		}
	}
	*pszTargetFound = szTargetFound;
	return true;
}

/* Polling for NFC targets
 * pnd - nfc_device_t struct pointer that represent currently used device
 * ppttTargetTypes - array of desired target types
 * szTargetTypes - ppttTargetTypes count
 * uiPollNr - specifies the number of polling
 *            (0x01-0xFE: 1 up to 254 polling, 0xFF: Endless polling)
 *            NOTE one polling is a polling for each desired target type
 * uiPeriod - indicates the polling period in units of 150 ms
 *            (0x01-0x0F: 150ms-2.25s)
 *            NOTE e.g. if uiPeriod=10, it will poll each desired target
 *            type during 1.5s
 * [out]pnt - pointer on nfc_target_t (over)writable struct
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 */
bool nfc_initiator_poll_target(nfc_device_t *pnd,
			       const nfc_modulation_t *pnmModulations,
			       const size_t szModulations,
			       const uint8_t uiPollNr,
			       const uint8_t uiPeriod,
			       nfc_target_t *pnt)
{
	if (pnd->driver->initiator_poll_target) {
		return pnd->driver->initiator_poll_target(pnd, pnmModulations,
							  szModulations,
							  uiPollNr, uiPeriod, pnt);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Select a target and request active or passive mode for D.E.P.
 * (Data Exchange Protocol)
 * pnd - nfc_device_t struct pointer that represent currently used device
 * ndm - desired D.E.P. mode (NDM_ACTIVE or NDM_PASSIVE for active,
 *       respectively passive mode)
 * ndiInitiator - pointer nfc_dep_info_t struct that contains NFCID3 and
 *                General Bytes to set to the initiator device (optionnal,
 *                can be NULL)
 * [out]pnt - is a nfc_target_t struct pointer where target information
 *            will be put.
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * The NFC device will try to find an available D.E.P. target. The
 * standards (ISO18092 and ECMA-340) describe the modulation that can be
 * used for reader to passive communications.
 * NOTE nfc_dep_info_t will be returned when the target was acquired
 *      successfully.
 */
bool nfc_initiator_select_dep_target(nfc_device_t *pnd,
				     const nfc_dep_mode_t ndm,
				     const nfc_baud_rate_t nbr,
				     const nfc_dep_info_t *pndiInitiator,
				     nfc_target_t *pnt)
{
	if (pnd->driver->initiator_select_dep_target) {
		return pnd->driver->initiator_select_dep_target(pnd, ndm, nbr,
								pndiInitiator,
								pnt);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Deselect a selected passive or emulated tag
 * pnd - nfc_device_t struct pointer that represents currently used device
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * After selecting and communicating with a passive tag, this function
 * could be used to deactivate and release the tag. This is very useful
 * when there are multiple tags available in the field. It is possible to
 * use the nfc_initiator_select_passive_target() function to select the
 * first available tag, test it for the available features and support,
 * deselect it and skip to the next tag until the correct tag is found.
 */
bool nfc_initiator_deselect_target(nfc_device_t *pnd)
{
	if (pnd->driver->initiator_deselect_target) {
		return pnd->driver->initiator_deselect_target(pnd);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Send data to target then retrieve data from target
 * pbtTx - contains a byte array of the frame that needs to be transmitted.
 * szTx - contains the length in bytes.
 * timeout - timeval struct pointer (NULL means infinite)
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * The NFC device (configured as initiator) will transmit the supplied
 * bytes (pbtTx) to the target.
 * It waits for the response and stores the received bytes in the pbtRx
 * byte array.
 * If timeout is not a null pointer, it specifies the maximum interval to
 * wait for the function to be executed.
 * If timeout is a null pointer, the function blocks indefinitely (until
 * an error is raised or function is completed).
 * If NDO_EASY_FRAMING option is disabled the frames will sent and
 * received in raw mode: PN53x will not handle input neither output data.
 * The parity bits are handled by the PN53x chip. The CRC can be generated
 * automatically or handled manually.
 * Using this function, frames can be communicated very fast via the NFC
 * initiator to the tag.
 * Tests show that on average this way of communicating is much faster
 * than using the regular driver/middle-ware (often supplied by
 * manufacturers).
 * WARNING The configuration option NDO_HANDLE_PARITY must be set to true
 *         (the default value).
 */
bool nfc_initiator_transceive_bytes(nfc_device_t *pnd,
				    const byte_t *pbtTx, const size_t szTx,
				    byte_t *pbtRx, size_t *pszRx,
				    struct timeval *timeout)
{
	if (pnd->driver->initiator_transceive_bytes) {
		return pnd->driver->initiator_transceive_bytes(pnd,
							       pbtTx, szTx,
							       pbtRx, pszRx,
							       timeout);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Transceive raw bit-frames to a target
 * pbtTx - contains a byte array of the frame that needs to be transmitted
 * szTxBits - contains the length in bits
 * NOTE For example the REQA (0x26) command (first anti-collision command
 *      of ISO14443-A) must be precise 7 bits long. This is not possible
 *      by using nfc_initiator_transceive_bytes(). With that function you
 *      can only communicate frames that consist of full bytes. When you
 *      send a full byte (8 bits + 1 parity) with the value of REQA
 *      (0x26), a tag will simply not respond.  More information about
 *      this can be found in the anti-collision example (nfc-anticol).
 * pbtTxPar - parameter contains a byte array of the corresponding parity
 *            bits needed to send per byte.
 * NOTE For example if you send the
 *      SELECT_ALL (0x93, 0x20) = [ 10010011, 00100000 ]
 *      command, you have to supply the following parity bytes
 *      (0x01, 0x00) to define the correct odd parity bits. This is only
 *      an example to explain how it works, if you just are sending two
 *      bytes with ISO14443-A compliant parity bits you better can use the
 *      nfc_initiator_transceive_bytes() function.
 * [out]pbtRx - response from the tag
 * [out]pszRxBits - pbtRx length in bits
 * [out]pbtRxPar - parameter contains a byte array of the corresponding
 *                 parity bits
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * The NFC device (configured as initiator) will transmit low-level
 * messages where only the modulation is handled by the PN53x chip.
 * Construction of the frame (data, CRC and parity) is completely done by
 * libnfc.  This can be very useful for testing purposes. Some protocols
 * (e.g. MIFARE Classic) require to violate the ISO14443-A standard by
 * sending incorrect parity and CRC bytes. Using this feature you are able
 * to simulate these frames.
 */
bool nfc_initiator_transceive_bits(nfc_device_t *pnd,
				   const byte_t *pbtTx,
				   const size_t szTxBits,
				   const byte_t *pbtTxPar,
				   byte_t *pbtRx,
				   size_t *pszRxBits,
				   byte_t *pbtRxPar)
{
	if (pnd->driver->initiator_transceive_bits) {
		return pnd->driver->initiator_transceive_bits(pnd,
							      pbtTx,
							      szTxBits,
							      pbtTxPar,
							      pbtRx,
							      pszRxBits,
							      pbtRxPar);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Send data to target then retrieve data from target
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function is similar to nfc_initiator_transceive_bytes() with the
 * following differences:
 * - A precise cycles counter will indicate the number of cycles between
 *   emission & reception of frames.
 * - It only supports mode with NDO_EASY_FRAMING option disabled.
 * - Overall communication with the host is heavier and slower.
 * Timer control:
 * By default timer configuration tries to maximize the precision, which
 * also limits the maximum cycles count before saturation/timeout.
 * E.g. with PN53x it can count up to 65535 cycles, so about 4.8ms, with a
 * precision of about 73ns.
 * - If you're ok with the defaults, set *cycles = 0 before calling this
 *   function.
 * - If you need to count more cycles, set *cycles to the maximum you
 *   expect but don't forget you'll loose in precision and it'll take more
 *   time before timeout, so don't abuse!
 * WARNING The configuration option NDO_EASY_FRAMING must be set to false.
 * WARNING The configuration option NDO_HANDLE_PARITY must be set to true
 *         (the default value).
 */
bool nfc_initiator_transceive_bytes_timed(nfc_device_t *pnd,
					  const byte_t *pbtTx,
					  const size_t szTx,
					  byte_t *pbtRx,
					  size_t *pszRx,
					  uint32_t *cycles)
{
	if (pnd->driver->initiator_transceive_bytes_timed) {
		return pnd->driver->initiator_transceive_bytes_timed(pnd,
								     pbtTx,
								     szTx,
								     pbtRx,
								     pszRx,
								     cycles);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Transceive raw bit-frames to a target
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function is similar to nfc_initiator_transceive_bits() with the
 * following differences:
 * - A precise cycles counter will indicate the number of cycles between
 *   emission & reception of frames.
 * - It only supports mode with NDO_EASY_FRAMING option disabled and CRC
 *   must be handled manually.
 * - Overall communication with the host is heavier and slower.
 * Timer control:
 * By default timer configuration tries to maximize the precision, which
 * also limits the maximum cycles count before saturation/timeout.
 * E.g. with PN53x it can count up to 65535 cycles, so about 4.8ms, with a
 * precision of about 73ns.
 * - If you're ok with the defaults, set *cycles = 0 before calling this
 *   function.
 * - If you need to count more cycles, set *cycles to the maximum you
 *   expect but don't forget
 *   you'll loose in precision and it'll take more time before timeout, so
 *   don't abuse!
 * WARNING The configuration option NDO_EASY_FRAMING must be set to false.
 * WARNING The configuration option NDO_HANDLE_CRC must be set to false.
 * WARNING The configuration option NDO_HANDLE_PARITY must be set to true
 *         (the default value).
 */
bool nfc_initiator_transceive_bits_timed(nfc_device_t *pnd,
					 const byte_t *pbtTx,
					 const size_t szTxBits,
					 const byte_t *pbtTxPar,
					 byte_t *pbtRx,
					 size_t *pszRxBits,
					 byte_t *pbtRxPar,
					 uint32_t *cycles)
{
	if (pnd->driver->initiator_transceive_bits_timed) {
		return pnd->driver->initiator_transceive_bits_timed(pnd,
								    pbtTx,
								    szTxBits,
								    pbtTxPar,
								    pbtRx,
								    pszRxBits,
								    pbtRxPar,
								    cycles);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Initialize NFC device as an emulated tag
 * pnd - nfc_device_t struct pointer that represent currently used device
 * ntm - target mode restriction that you want to emulate
 *       (eg. NTM_PASSIVE_ONLY)
 * pnt - pointer to nfc_target_t struct that represents the wanted
 *       emulated target
 *       NOTE pnt can be updated by this function: if you set
 *       NBR_UNDEFINED and/or NDM_UNDEFINED (ie. for DEP mode), these
 *       fields will be updated.
 * [out]pbtRx - Rx buffer pointer
 * [out]pszRx - received bytes count
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function initializes NFC device in target mode in order to emulate
 * a tag using the specified nfc_target_mode_t.
 * - Crc is handled by the device (NDO_HANDLE_CRC = true)
 * - Parity is handled the device (NDO_HANDLE_PARITY = true)
 * - Cryto1 cipher is disabled (NDO_ACTIVATE_CRYPTO1 = false)
 * - Auto-switching in ISO14443-4 mode is enabled
 *   (NDO_AUTO_ISO14443_4 = true)
 * - Easy framing is disabled (NDO_EASY_FRAMING = false)
 * - Invalid frames are not accepted (NDO_ACCEPT_INVALID_FRAMES = false)
 * - Multiple frames are not accepted (NDO_ACCEPT_MULTIPLE_FRAMES = false)
 * - RF field is dropped
 * WARNING Be aware that this function will wait (hang) until a command is
 *         received that is not part of the anti-collision. The RATS
 *         command for example would wake up the emulator. After this is
 *         received, the send and receive functions can be used.
 */
bool nfc_target_init(nfc_device_t *pnd, nfc_target_t *pnt,
		     byte_t *pbtRx, size_t *pszRx)
{
	/* Disallow invalid frame */
	if (!nfc_configure (pnd, NDO_ACCEPT_INVALID_FRAMES, false))
		return false;
	/* Disallow multiple frames */
	if (!nfc_configure (pnd, NDO_ACCEPT_MULTIPLE_FRAMES, false))
		return false;
	/* Make sure we reset the CRC and parity to chip handling. */
	if (!nfc_configure (pnd, NDO_HANDLE_CRC, true))
		return false;
	if (!nfc_configure (pnd, NDO_HANDLE_PARITY, true))
		return false;
	/* Activate auto ISO14443-4 switching by default */
	if (!nfc_configure (pnd, NDO_AUTO_ISO14443_4, true))
		return false;
	/* Activate "easy framing" feature by default */
	if (!nfc_configure (pnd, NDO_EASY_FRAMING, true))
		return false;
	/* Deactivate the CRYPTO1 cipher, it may could cause problems
	 * when still active
	 */
	if (!nfc_configure(pnd, NDO_ACTIVATE_CRYPTO1, false))
		return false;
	/* Drop explicitely the field */
	if (!nfc_configure (pnd, NDO_ACTIVATE_FIELD, false))
		return false;
	if (pnd->driver->target_init) {
		return pnd->driver->target_init(pnd, pnt, pbtRx, pszRx);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Turn NFC device in idle mode
 * pnd - nfc_device_t struct pointer that represent currently used device
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function switch the device in idle mode.  In initiator mode, the
 * RF field is turned off and the device is set to low power mode (if
 * avaible);  In target mode, the emulation is stoped (no target available
 * from external initiator) and the device is set to low power mode (if
 * avaible).
 */
bool nfc_idle(nfc_device_t *pnd)
{
	if (pnd->driver->idle) {
		return pnd->driver->idle(pnd);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Abort current running command
 * pnd - nfc_device_t struct pointer that represent currently used device
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * Some commands (ie. nfc_target_init()) are blocking functions and will
 * return only in particular conditions (ie. external initiator request).
 * This function attempt to abort the current running command.
 * NOTE The blocking function (ie. nfc_target_init()) will failed with
 *      DEABORT error.
 */
bool nfc_abort_command(nfc_device_t *pnd)
{
	if (pnd->driver->abort_command) {
		return pnd->driver->abort_command(pnd);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Send bytes and APDU frames
 * pnd - nfc_device_t struct pointer that represent currently used device
 * pbtTx - pointer to Tx buffer
 * szTx - size of Tx buffer
 * timeout - timeval struct pointer (NULL means infinite)
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function make the NFC device (configured as target) send byte
 * frames (e.g. APDU responses) to the initiator.
 * If timeout is not a null pointer, it specifies the maximum interval to
 * wait for the function to be executed.
 * If timeout is a null pointer, the function blocks indefinitely (until
 * an error is raised or function is completed).
 */
bool nfc_target_send_bytes(nfc_device_t *pnd, const byte_t *pbtTx,
			   const size_t szTx, struct timeval *timeout)
{
	if (pnd->driver->target_send_bytes) {
		return pnd->driver->target_send_bytes(pnd, pbtTx, szTx, timeout);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Receive bytes and APDU frames
 * pnd - nfc_device_t struct pointer that represent currently used device
 * [out]pbtRx - pointer to Rx buffer
 * [out]pszRx - received byte count
 * timeout - timeval struct pointer (NULL means infinite)
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function retrieves bytes frames (e.g. ADPU) sent by the initiator
 * to the NFC device (configured as target).
 * If timeout is not a null pointer, it specifies the maximum interval to
 * wait for the function to be executed.
 * If timeout is a null pointer, the function blocks indefinitely (until
 * an error is raised or function is completed).
 */
bool nfc_target_receive_bytes(nfc_device_t *pnd, byte_t *pbtRx,
			      size_t *pszRx, struct timeval *timeout)
{
	if (pnd->driver->target_receive_bytes) {
		return pnd->driver->target_receive_bytes(pnd, pbtRx, pszRx, timeout);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Send raw bit-frames
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function can be used to transmit (raw) bit-frames to the initiator
 * using the specified NFC device (configured as target).
 */
bool nfc_target_send_bits(nfc_device_t *pnd, const byte_t *pbtTx,
			  const size_t szTxBits, const byte_t *pbtTxPar)
{
	if (pnd->driver->target_send_bits) {
		return pnd->driver->target_send_bits(pnd, pbtTx, szTxBits, pbtTxPar);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

/* Receive bit-frames
 * Returns true if action was successfully performed;
 *         otherwise returns false.
 * This function makes it possible to receive (raw) bit-frames.  It
 * returns all the messages that are stored in the FIFO buffer of the
 * PN53x chip.  It does not require to send any frame and thereby could be
 * used to snoop frames that are transmitted by a nearby initiator.
 * NOTE Check out the NDO_ACCEPT_MULTIPLE_FRAMES configuration option to
 *      avoid losing transmitted frames.
 */
bool nfc_target_receive_bits(nfc_device_t *pnd, byte_t *pbtRx,
			     size_t *pszRxBits, byte_t *pbtRxPar)
{
	if (pnd->driver->target_receive_bits) {
		return pnd->driver->target_receive_bits(pnd, pbtRx,
							pszRxBits,
							pbtRxPar);
	} else {
		pnd->iLastError = EDEVNOTSUP;
		return false;
	}
}

const char *nfc_strerror(const nfc_device_t *pnd)
{
	return pnd->driver->strerror(pnd);
}

int nfc_strerror_r(const nfc_device_t *pnd, char *pcStrErrBuf, size_t szBufLen)
{
	return (snprintf(pcStrErrBuf, szBufLen, "%s", nfc_strerror (pnd)) < 0) ? -1 : 0;
}

void nfc_perror(const nfc_device_t *pnd, const char *pcString)
{
	fprintf(stderr, "%s: %s\n", pcString, nfc_strerror(pnd));
}

const char *nfc_device_name(nfc_device_t *pnd)
{
	return pnd->acName;
}

const char *nfc_version(void)
{
	return "1.5.1";
}
