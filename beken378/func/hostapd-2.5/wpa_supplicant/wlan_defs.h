/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NET_WLAN_WLAN_DEFS_H_
#define _NET_WLAN_WLAN_DEFS_H_

#include <stdint.h>
#include <defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief WLAN SSID and passphrase definition
 */
#define WLAN_SSID_MAX_LEN       32
#define WLAN_PASSPHRASE_MIN_LEN 8
#define WLAN_PASSPHRASE_MAX_LEN 63
#define WLAN_PSK_HEX_LEN        32
#define WLAN_PSK_HEX_STR_LEN    64 /* two characters per octet of PSK */

/**
 * @brief WLAN WEP key length definition
 */
#define WLAN_WEP40_KEY_LEN      5  /* 5-byte  (40-bit)  */
#define WLAN_WEP104_KEY_LEN     13 /* 13-byte (104-bit) */
#define WLAN_WEP128_KEY_LEN     16 /* 16-byte (128-bit), unsupported */
#define WLAN_WEP_KEY_MAX_LEN    WLAN_WEP128_KEY_LEN

#ifndef ETH_ALEN
#define ETH_ALEN		6
#endif

/**
 * @brief Wlan ssid definition
 */
typedef struct wlan_ssid {
	uint8_t ssid[WLAN_SSID_MAX_LEN];
	uint8_t ssid_len;
} wlan_ssid_t;

/**
 * @brief Wlan station configuration field definition
 */
typedef enum wlan_sta_field {
	WLAN_STA_FIELD_SSID = 0,
	WLAN_STA_FIELD_BSSID,
	WLAN_STA_FIELD_PSK,
	WLAN_STA_FIELD_WEP_KEY0,
	WLAN_STA_FIELD_WEP_KEY1,
	WLAN_STA_FIELD_WEP_KEY2,
	WLAN_STA_FIELD_WEP_KEY3,
	WLAN_STA_FIELD_WEP_KEY_INDEX,
	WLAN_STA_FIELD_KEY_MGMT,
	WLAN_STA_FIELD_PAIRWISE_CIPHER,
	WLAN_STA_FIELD_GROUP_CIPHER,
	WLAN_STA_FIELD_PROTO,
	WLAN_STA_FIELD_AUTH_ALG,
	WLAN_STA_FIELD_WPA_PTK_REKEY,
	WLAN_STA_FIELD_SCAN_SSID,
	WLAN_STA_FIELD_FAST_CONNECT,
	WLAN_STA_FIELD_FREQ,	/* only used in fast connect */

	WLAN_STA_FIELD_NUM,
} wlan_sta_field_t;

/**
 * @brief Wlan station configuration definition
 */
typedef struct wlan_sta_config {
	wlan_sta_field_t field;
#ifdef CONFIG_WPA_SUPPLICANT_MULTI_NETWORK
	int id;	/* network id */
#endif

	union {
		/**
		 * Network name
		 */
		wlan_ssid_t ssid;

		/**
		 * bssid - BSSID
		 *
		 * If set, this network block is used only when associating with the AP
		 * using the configured BSSID
		 */
		u8 bssid[ETH_ALEN];

		/**
		 * fast_connect: connect without scan
		 */
		u8 fast_connect;

		/**
		 * fast_connect_freq
		 */
		int channel;

		/**
		 * WPA preshared key in one of the optional formats:
		 *   - an ASCII string of passphrase, length is [8, 63]
		 *   - a hex string of PSK (two characters per octet of PSK), length is 64
		 */
		uint8_t psk[65];

		/**
		 * WEP key in one of the optional formats:
		 *   - an ASCII string with double quotation, length is {5, 13}
		 *   - a hex string (two characters per octet), length is {10, 26}
		 */
		uint8_t wep_key[WLAN_WEP_KEY_MAX_LEN * 2 + 1];

		/**
		 * Default key index for TX frames using WEP
		 */
		int wep_tx_keyidx;

		/**
		 * Bitfield of allowed key management protocols
		 *
		 * WPA_KEY_MGMT_*
		 */
		int key_mgmt;

		/**
		 * Bitfield of allowed pairwise ciphers
		 *
		 * WPA_CIPHER_*
		 */
		int pairwise_cipher;

		/**
		 * Bitfield of allowed group ciphers
		 *
		 * WPA_CIPHER_*
		 */
		int group_cipher;

		/**
		 * Bitfield of allowed protocols
		 *
		 * WPA_PROTO_*
		 */
		int proto;

		/**
		 * Bitfield of allowed authentication algorithms
		 *
		 * WPA_AUTH_ALG_*
		 */
		int auth_alg;

		/**
		 * Maximum lifetime for PTK in seconds
		 *
		 * This value can be used to enforce rekeying of PTK to
		 * mitigate some attacks against TKIP deficiencies.
		 */
		int wpa_ptk_rekey;

		/**
		 * Scan this SSID with Probe Requests
		 *
		 * scan_ssid can be used to scan for APs using hidden SSIDs.
		 */
		int scan_ssid;
	} u;
} wlan_sta_config_t;

/**
 * @brief Wlan station connection state definition
 */
typedef enum wlan_sta_states {
	WLAN_STA_STATE_DISCONNECTED = 0,
	WLAN_STA_STATE_CONNECTED = 1,
} wlan_sta_states_t;

/**
 * @brief Wlan AP information definition
 */
typedef struct wlan_sta_ap {
	wlan_ssid_t	ssid;
	uint8_t		bssid[6];
	uint8_t		channel;
	uint16_t	beacon_int;
	int		freq;
	int		rssi;
	int		level;
	int		wpa_flags;
	int		wpa_cipher;
	int		wpa_key_mgmt;
	int		wpa2_cipher;
	int		wpa2_key_mgmt;
} wlan_sta_ap_t;

/**
 * @brief Wlan station scan parameters definition
 */
typedef struct wlan_sta_scan_param {
	uint8_t scan_only;    /* do scan only */
	uint8_t scan_passive; /* passive scan */
	uint8_t scan_ssid;    /* Scan SSID of configured network with Probe Requests */
} wlan_sta_scan_param_t;

/**
 * @brief Wlan station scan results definition
 */
typedef struct wlan_sta_scan_results {
	wlan_sta_ap_t *ap;
	int size;
	int num;
} wlan_sta_scan_results_t;

/**
 * @brief Wlan station bss infomation definition
 */
typedef struct wlan_sta_bss_info {
	uint8_t *bss;
	uint32_t size;
} wlan_sta_bss_info_t;

/**
 * @brief Parameter of generating WPA PSK based on passphrase and SSID
 */
typedef struct wlan_gen_psk_param {
	uint8_t ssid[WLAN_SSID_MAX_LEN];
	uint8_t ssid_len;
	char passphrase[WLAN_PASSPHRASE_MAX_LEN + 1];
	uint8_t psk[WLAN_PSK_HEX_LEN]; /* out */
} wlan_gen_psk_param_t;

/**
 * @brief Wlan WPS pin definition
 */
typedef struct wlan_sta_wps_pin {
	uint8_t pin[9];
} wlan_sta_wps_pin_t;

/**
 * @brief Wlan AP configuration field definition
 */
typedef enum wlan_ap_field {
	WLAN_AP_FIELD_SSID = 0,
	WLAN_AP_FIELD_PSK,
	WLAN_AP_FIELD_KEY_MGMT,
	WLAN_AP_FIELD_WPA_CIPHER,
	WLAN_AP_FIELD_RSN_CIPHER,
	WLAN_AP_FIELD_PROTO,
	WLAN_AP_FIELD_AUTH_ALG,
	WLAN_AP_FIELD_GROUP_REKEY,
	WLAN_AP_FIELD_STRICT_REKEY,
	WLAN_AP_FIELD_GMK_REKEY,
	WLAN_AP_FIELD_PTK_REKEY,
	WLAN_AP_FIELD_HW_MODE,
	WLAN_AP_FIELD_IEEE80211N,
	WLAN_AP_FIELD_CHANNEL,
	WLAN_AP_FIELD_BEACON_INT,
	WLAN_AP_FIELD_DTIM,
	WLAN_AP_FIELD_MAX_NUM_STA,

	WLAN_AP_FIELD_NUM,
} wlan_ap_field_t;

/**
 * @brief Wlan AP hardware mode definition
 */
typedef enum wlan_ap_hw_mode {
	WLAN_AP_HW_MODE_IEEE80211B = 0,
	WLAN_AP_HW_MODE_IEEE80211G,
	WLAN_AP_HW_MODE_IEEE80211A,
	WLAN_AP_HW_MODE_IEEE80211AD,

	WLAN_AP_HW_MODE_NUM,
} wlan_ap_hw_mode_t;

/**
 * @brief Wlan AP configuration definition
 */
typedef struct wlan_ap_config {
	wlan_ap_field_t field;

	union {
		/**
		 * Network name
		 */
		wlan_ssid_t ssid;

		/**
		 * WPA preshared key in one of the optional formats:
		 *   - an ASCII string of passphrase, length is [8, 63]
		 *   - a hex string of PSK (two characters per octet of PSK), length is 64
		 */
		uint8_t psk[65];

		/**
		 * Bitfield of allowed key management protocols
		 *
		 * WPA_KEY_MGMT_*
		 */
		int key_mgmt;

		/**
		 * Bitfield of allowed WPA pairwise ciphers
		 *
		 * WPA_CIPHER_*
		 */
		int wpa_cipher;

		/**
		 * Bitfield of allowed RSN pairwise ciphers
		 *
		 * WPA_CIPHER_*
		 */
		int rsn_cipher;

		/**
		 * Bitfield of allowed protocols
		 *
		 * WPA_PROTO_*
		 */
		int proto;

		/**
		 * Bitfield of allowed authentication algorithms
		 *
		 * WPA_AUTH_ALG_*
		 */
		int auth_alg;

		/**
		 * Maximum lifetime for GTK in seconds
		 */
		int group_rekey;

		/**
		 * Rekey GTK when any STA that possesses the current GTK is
		 * leaving the BSS
		 */
		int strict_rekey;

		/**
		 * Maximum lifetime for GMK in seconds
		 */
		int gmk_rekey;

		/**
		 * Maximum lifetime for PTK in seconds
		 */
		int ptk_rekey;

		/**
		 * Hardware mode
		 */
		wlan_ap_hw_mode_t hw_mode;

		/**
		 * IEEE802.11n mode
		 */
		int ieee80211n;

		/**
		 * RF channel
		 */
		uint8_t channel;

		/**
		 * MIB defines range as 1..65535, but very small values
		 * cause problems with the current implementation.
		 * Since it is unlikely that this small numbers are
		 * useful in real life scenarios, do not allow beacon
		 * period to be set below 15 TU.
		 */
		uint16_t beacon_int;

		/**
		 * Delivery traffic indication message
		 */
		int dtim;

		/**
		 * Maximum number of STAs in station table
		 */
		int max_num_sta;
	} u;
} wlan_ap_config_t;

/**
 * @brief Wlan station information definition
 */
typedef struct wlan_ap_sta {
	uint8_t addr[6];
} wlan_ap_sta_t;

/**
 * @brief Wlan connected stations information definition
 */
typedef struct wlan_ap_stas {
	wlan_ap_sta_t *sta;
	int size;
	int num;
} wlan_ap_stas_t;

#ifdef __cplusplus
}
#endif

#endif /* _NET_WLAN_WLAN_DEFS_H_ */

