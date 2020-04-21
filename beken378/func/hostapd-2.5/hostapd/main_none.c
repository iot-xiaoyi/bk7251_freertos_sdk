/*
 * WPA Supplicant / Example program entrypoint
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "include.h"
#include "uart_pub.h"
#include "includes.h"
#include "hostapd_cfg.h"
#include "common.h"
#include "ap/hostapd.h"
#include "eloop.h"
#include "main_none.h"
#include "ap/sta_info.h"
#include "ps.h"
#include "param_config.h"
#include "wlan_ui_pub.h"
#include "rtos_pub.h"
#include "error.h"
#include "wlan_ui_pub.h"
#if CFG_NEW_SUPP
#include "wpa_ctrl.h"
#endif

beken_thread_t  hostapd_thread_handle = NULL;
extern beken_thread_t wpas_thread_handle;
uint32_t  hostapd_stack_size = 4000;
beken_semaphore_t hostapd_sema = NULL;
beken_semaphore_t wpa_hostapd_sema = NULL;
void *wpa_hostapd_param = NULL;
beken_queue_t wpah_queue = NULL;
static struct hapd_global s_hapd_global;
struct hapd_interfaces g_hapd_interfaces;

char *bss_iface = "wlan0";

extern int ap_channel_switch(struct hostapd_iface *ap_iface, int new_freq);

#if CFG_NEW_SUPP
struct hapd_interfaces *hostapd_ctrl_get_interfaces()
{
	return &g_hapd_interfaces;
}
#endif

void hostapd_cfg_defaults_bss(struct hostapd_bss_config *bss)
{
	bss->logger_syslog_level = HOSTAPD_LEVEL_INFO;
	bss->logger_stdout_level = HOSTAPD_LEVEL_INFO;
	bss->logger_syslog = (unsigned int) -1;
	bss->logger_stdout = (unsigned int) -1;

	bss->auth_algs = CFG_AUTH_ALG;

	bss->wep_rekeying_period = 300;
	/* use key0 in individual key and key1 in broadcast key */
	bss->broadcast_key_idx_min = 1;
	bss->broadcast_key_idx_max = 2;
	bss->eap_reauth_period = 3600;

	bss->wpa_group_rekey = 600;
	bss->wpa_gmk_rekey = 86400;
	bss->wpa_key_mgmt = WPA_KEY_MGMT_PSK;
	bss->wpa_pairwise = WPA_CIPHER_TKIP;
	bss->wpa_group = WPA_CIPHER_TKIP;
	bss->rsn_pairwise = 0;

	bss->max_num_sta = CFG_SUPPORTED_MAX_STA_NUM;

	bss->dtim_period = 2;

	bss->radius_server_auth_port = 1812;
	bss->ap_max_inactivity = AP_MAX_INACTIVITY;
	bss->eapol_version = 2;

	bss->max_listen_interval = 65535;

	bss->pwd_group = 19; /* ECC: GF(p=256) */

#ifdef CONFIG_IEEE80211W
	bss->assoc_sa_query_max_timeout = 1000;
	bss->assoc_sa_query_retry_timeout = 201;
	bss->group_mgmt_cipher = WPA_CIPHER_AES_128_CMAC;
#endif /* CONFIG_IEEE80211W */
#ifdef EAP_SERVER_FAST
	 /* both anonymous and authenticated provisioning */
	bss->eap_fast_prov = 3;
	bss->pac_key_lifetime = 7 * 24 * 60 * 60;
	bss->pac_key_refresh_time = 1 * 24 * 60 * 60;
#endif /* EAP_SERVER_FAST */

	/* Set to -1 as defaults depends on HT in setup */
	bss->wmm_enabled = -1;

#ifdef CONFIG_IEEE80211R
	bss->ft_over_ds = 1;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_SAE
	bss->sae_anti_clogging_threshold = 5;
#endif
}

//hostapd_config_defaults
struct hostapd_config * hostapd_cfg_defaults(void)
{
#define ecw2cw(ecw) ((1 << (ecw)) - 1)

	struct hostapd_config *conf;
	struct hostapd_bss_config *bss;
	const int aCWmin = 4, aCWmax = 10;
	const struct hostapd_wmm_ac_params ac_bk =
		{ aCWmin, aCWmax, 7, 0, 0 }; /* background traffic */
	const struct hostapd_wmm_ac_params ac_be =
		{ aCWmin, aCWmax, 3, 0, 0 }; /* best effort traffic */
	const struct hostapd_wmm_ac_params ac_vi = /* video traffic */
		{ aCWmin - 1, aCWmin, 2, 3008 / 32, 0 };
	const struct hostapd_wmm_ac_params ac_vo = /* voice traffic */
		{ aCWmin - 2, aCWmin - 1, 2, 1504 / 32, 0 };
	const struct hostapd_tx_queue_params txq_bk =
		{ 7, ecw2cw(aCWmin), ecw2cw(aCWmax), 0 };
	const struct hostapd_tx_queue_params txq_be =
		{ 3, ecw2cw(aCWmin), 4 * (ecw2cw(aCWmin) + 1) - 1, 0};
	const struct hostapd_tx_queue_params txq_vi =
		{ 1, (ecw2cw(aCWmin) + 1) / 2 - 1, ecw2cw(aCWmin), 30};
	const struct hostapd_tx_queue_params txq_vo =
		{ 1, (ecw2cw(aCWmin) + 1) / 4 - 1,
		  (ecw2cw(aCWmin) + 1) / 2 - 1, 15};

#undef ecw2cw

	conf = os_zalloc(sizeof(*conf));
	bss = os_zalloc(sizeof(*bss));
	if (conf == NULL || bss == NULL) {
		wpa_printf(MSG_ERROR, "Failed to allocate memory for "
			   "configuration data.");
		os_free(conf);
		os_free(bss);
		return NULL;
	}
	conf->bss = os_calloc(1, sizeof(struct hostapd_bss_config *));
	if (conf->bss == NULL) {
		os_free(conf);
		os_free(bss);
		return NULL;
	}
	conf->bss[0] = bss;

	hostapd_cfg_defaults_bss(bss);

	conf->num_bss = 1;

    #if CFG_SUPPORT_80211G
    conf->hw_mode = HOSTAPD_MODE_IEEE80211G;
    #endif

	conf->beacon_int           = 100;
	conf->rts_threshold        = -1; /* use driver default: 2347 */
	conf->fragm_threshold      = -1; /* user driver default: 2346 */
	conf->send_probe_response  = 1;
	/* Set to invalid value means do not add Power Constraint IE */
	conf->local_pwr_constraint = -1;

	conf->wmm_ac_params[0] = ac_be;
	conf->wmm_ac_params[1] = ac_bk;
	conf->wmm_ac_params[2] = ac_vi;
	conf->wmm_ac_params[3] = ac_vo;

	conf->tx_queue[0] = txq_vo;
	conf->tx_queue[1] = txq_vi;
	conf->tx_queue[2] = txq_be;
	conf->tx_queue[3] = txq_bk;

	#if CFG_AP_SUPPORT_HT_IE
	conf->ht_capab = HT_CAP_INFO_SMPS_DISABLED | HT_CAP_INFO_SHORT_GI20MHZ
						| HT_CAP_INFO_TX_STBC
						| HT_CAP_INFO_RX_STBC_1
						| HT_CAP_INFO_MAX_AMSDU_SIZE;
	#else
	conf->ht_capab = HT_CAP_INFO_SMPS_DISABLED;
	#endif

	conf->ap_table_max_size = 255;
	conf->ap_table_expiration_time = 60;
	conf->track_sta_max_age = 0;//180;

	conf->acs = 0;
	conf->acs_ch_list.num = 0;
#ifdef CONFIG_ACS
	conf->acs_num_scans = 5;
#endif /* CONFIG_ACS */

	return conf;
}

struct hostapd_config *hostapd_config_read(const char *fname)
{
	struct hostapd_config *conf = 0;
	int i;
	int errors = 0;
	struct hostapd_bss_config *bss;

	conf = hostapd_cfg_defaults();	// use hostapd_config_defaults
	if (NULL == conf)
		return NULL;

	conf->last_bss = conf->bss[0];
	bss = conf->last_bss;

	os_strcpy(bss->iface, bss_iface);
	wifi_get_mac_address((u8 *)&bss->bssid, CONFIG_ROLE_AP);
	/* set default driver based on configuration */
	conf->driver = wpa_drivers[0];
	conf->last_bss = conf->bss[0];

	#if CFG_AP_SUPPORT_HT_IE
	conf->ieee80211n = 1;
	conf->ht_capab = HT_CAP_INFO_SMPS_DISABLED | HT_CAP_INFO_SHORT_GI20MHZ
						| HT_CAP_INFO_TX_STBC
						| HT_CAP_INFO_RX_STBC_1
						| HT_CAP_INFO_MAX_AMSDU_SIZE;
	#endif

	bss->ssid.ssid_len = g_ap_param_ptr->ssid.length;
	os_memcpy(bss->ssid.ssid, g_ap_param_ptr->ssid.array, bss->ssid.ssid_len);
	bss->max_listen_interval = 65535;
	bss->ieee802_1x = 0;
	bss->ssid.ssid_set = 1;

	if (g_ap_param_ptr->cipher_suite == BK_SECURITY_TYPE_WEP) {
		bss->default_wep_key_len = 0;
		bss->ssid.wep.keys_set = 1;
		bss->ssid.wep.default_len = 10;
		bss->ssid.wep.idx = 0;
		bss->ssid.wep.len[0] = 5;
		bss->ssid.wep.key[0] = (u8 *)os_malloc(bss->ssid.wep.len[0]);
		if (bss->ssid.wep.key[0]) {
			int wkey;
			const char *wep_key = (char *)g_ap_param_ptr->key;
			if (g_ap_param_ptr->key_len == 5) {
				os_memcpy(bss->ssid.wep.key[0], wep_key, g_ap_param_ptr->key_len);
			} else if (g_ap_param_ptr->key_len == 10) {
				for (i = 0; i < bss->ssid.wep.len[0]; i ++) {
					wkey = hex2byte(&wep_key[2 * i]);
					ASSERT(-1 != wkey);

					bss->ssid.wep.key[0][i] = wkey;
				}
			} else {
				os_printf("WEP_KEY_len_exception\r\n");
			}
		}
	} else if (g_ap_param_ptr->cipher_suite == BK_SECURITY_TYPE_WPA_TKIP) {
		bss->wpa = 1;
		bss->wpa_pairwise = WPA_CIPHER_TKIP;
	} else if (g_ap_param_ptr->cipher_suite == BK_SECURITY_TYPE_WPA2_AES) {
		bss->wpa = 2;
		bss->wpa_pairwise = WPA_CIPHER_CCMP;
	} else if (g_ap_param_ptr->cipher_suite == BK_SECURITY_TYPE_WPA2_MIXED) {
		bss->wpa = 2;
		bss->wpa_pairwise = WPA_CIPHER_TKIP | WPA_CIPHER_CCMP;
	}
	if (g_ap_param_ptr->cipher_suite > BK_SECURITY_TYPE_WEP) {
		const char *wpa_key = (char *)g_ap_param_ptr->key;
		os_free(bss->ssid.wpa_passphrase);
		bss->ssid.wpa_passphrase = os_strdup(wpa_key);
		if (bss->ssid.wpa_passphrase) {
			hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
			bss->ssid.wpa_passphrase_set = 1;
		}
		bss->wpa_key_mgmt = WPA_KEY_MGMT_PSK;
	}


	for (i = 0; i < conf->num_bss; i++)
		hostapd_set_security_params(conf->bss[i], 1);

	if (hostapd_config_check(conf, 1))
		errors++;

	return conf;
}

#if CFG_NEW_SUPP
static int hostapd_config_parse_key_mgmt(int line, const char *value)
{
	int val = 0, last;
	char *start, *end, *buf;

	buf = os_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		if (os_strcmp(start, "WPA-PSK") == 0)
			val |= WPA_KEY_MGMT_PSK;
		else if (os_strcmp(start, "WPA-EAP") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X;
#ifdef CONFIG_SAE
		else if (os_strcmp(start, "SAE") == 0)
			val |= WPA_KEY_MGMT_SAE;
		else if (os_strcmp(start, "FT-SAE") == 0)
			val |= WPA_KEY_MGMT_FT_SAE;
#endif /* CONFIG_SAE */
		else {
			wpa_printf(MSG_ERROR, "Line %d: invalid key_mgmt '%s'",
				   line, start);
			os_free(buf);
			return -1;
		}

		if (last)
			break;
		start = end + 1;
	}

	os_free(buf);
	if (val == 0) {
		wpa_printf(MSG_ERROR, "Line %d: no key_mgmt values "
			   "configured.", line);
		return -1;
	}

	return val;
}

static int hostapd_config_parse_cipher(int line, const char *value)
{
	int val = wpa_parse_cipher(value);
	if (val < 0) {
		wpa_printf(MSG_ERROR, "Line %d: invalid cipher '%s'.",
			   line, value);
		return -1;
	}
	if (val == 0) {
		wpa_printf(MSG_ERROR, "Line %d: no cipher values configured.",
			   line);
		return -1;
	}
	return val;
}

static int hostapd_config_read_wep(struct hostapd_wep_keys *wep, int keyidx, char *val)
{
	size_t len = os_strlen(val);

	if (keyidx < 0 || keyidx > 3 || wep->key[keyidx] != NULL)
		return -1;

	if (val[0] == '"') {
		if (len < 2 || val[len - 1] != '"')
			return -1;
		len -= 2;
		wep->key[keyidx] = os_malloc(len);
		if (wep->key[keyidx] == NULL)
			return -1;
		os_memcpy(wep->key[keyidx], val + 1, len);
		wep->len[keyidx] = len;
	} else {
		if (len & 1)
			return -1;
		len /= 2;
		wep->key[keyidx] = os_malloc(len);
		if (wep->key[keyidx] == NULL)
			return -1;
		wep->len[keyidx] = len;
		if (hexstr2bin(val, wep->key[keyidx], len) < 0)
			return -1;
	}

	wep->keys_set++;

	return 0;
}

static int hostapd_parse_chanlist(struct hostapd_config *conf, char *val)
{
	char *pos;

	/* for backwards compatibility, translate ' ' in conf str to ',' */
	pos = val;
	while (pos) {
		pos = os_strchr(pos, ' ');
		if (pos)
			*pos++ = ',';
	}
	if (freq_range_list_parse(&conf->acs_ch_list, val))
		return -1;

	return 0;
}

static int hostapd_parse_intlist(int **int_list, char *val)
{
	int *list;
	int count;
	char *pos, *end;

	os_free(*int_list);
	*int_list = NULL;

	pos = val;
	count = 0;
	while (*pos != '\0') {
		if (*pos == ' ')
			count++;
		pos++;
	}

	list = os_malloc(sizeof(int) * (count + 2));
	if (list == NULL)
		return -1;
	pos = val;
	count = 0;
	while (*pos != '\0') {
		end = os_strchr(pos, ' ');
		if (end)
			*end = '\0';

		list[count++] = atoi(pos);
		if (!end)
			break;
		pos = end + 1;
	}
	list[count] = -1;

	*int_list = list;
	return 0;
}

static int hostapd_config_fill(struct hostapd_config *conf,
			       struct hostapd_bss_config *bss,
			       const char *buf, char *pos, int line)
{
	if (os_strcmp(buf, "ssid") == 0) {
		bss->ssid.ssid_len = os_strlen(pos);
		if (bss->ssid.ssid_len > SSID_MAX_LEN ||
		    bss->ssid.ssid_len < 1) {
			wpa_printf(MSG_ERROR, "Line %d: invalid SSID '%s'",
				   line, pos);
			return 1;
		}
		os_memcpy(bss->ssid.ssid, pos, bss->ssid.ssid_len);
		bss->ssid.ssid_set = 1;
	} else if (os_strcmp(buf, "utf8_ssid") == 0) {
		bss->ssid.utf8_ssid = atoi(pos) > 0;
	} else if (os_strcmp(buf, "ap_isolate") == 0) {
		bss->isolate = atoi(pos);
	} else if (os_strcmp(buf, "ap_max_inactivity") == 0) {
		bss->ap_max_inactivity = atoi(pos);
	} else if (os_strcmp(buf, "skip_inactivity_poll") == 0) {
		bss->skip_inactivity_poll = atoi(pos);
	} else if (os_strcmp(buf, "country_code") == 0) {
		os_memcpy(conf->country, pos, 2);
		/* FIX: make this configurable */
		conf->country[2] = ' ';
	} else if (os_strcmp(buf, "ieee80211d") == 0) {
		conf->ieee80211d = atoi(pos);
	} else if (os_strcmp(buf, "ieee80211h") == 0) {
		conf->ieee80211h = atoi(pos);
	} else if (os_strcmp(buf, "eapol_version") == 0) {
		bss->eapol_version = atoi(pos);
		if (bss->eapol_version < 1 || bss->eapol_version > 2) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid EAPOL version (%d): '%s'.",
				   line, bss->eapol_version, pos);
			return 1;
		}
		wpa_printf(MSG_DEBUG, "eapol_version=%d", bss->eapol_version);
	} else if (os_strcmp(buf, "wep_rekey_period") == 0) {
		bss->wep_rekeying_period = atoi(pos);
		if (bss->wep_rekeying_period < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid period %d",
				   line, bss->wep_rekeying_period);
			return 1;
		}
	} else if (os_strcmp(buf, "eap_reauth_period") == 0) {
		bss->eap_reauth_period = atoi(pos);
		if (bss->eap_reauth_period < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid period %d",
				   line, bss->eap_reauth_period);
			return 1;
		}
	} else if (os_strcmp(buf, "eapol_key_index_workaround") == 0) {
		bss->eapol_key_index_workaround = atoi(pos);
	} else if (os_strcmp(buf, "auth_algs") == 0) {
		bss->auth_algs = atoi(pos);
		if (bss->auth_algs == 0) {
			wpa_printf(MSG_ERROR, "Line %d: no authentication algorithms allowed",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "max_num_sta") == 0) {
		bss->max_num_sta = atoi(pos);
		if (bss->max_num_sta < 0 ||
		    bss->max_num_sta > MAX_STA_COUNT) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid max_num_sta=%d; allowed range 0..%d",
				   line, bss->max_num_sta, MAX_STA_COUNT);
			return 1;
		}
	} else if (os_strcmp(buf, "wpa") == 0) {
		bss->wpa = atoi(pos);
	} else if (os_strcmp(buf, "wpa_group_rekey") == 0) {
		bss->wpa_group_rekey = atoi(pos);
	} else if (os_strcmp(buf, "wpa_strict_rekey") == 0) {
		bss->wpa_strict_rekey = atoi(pos);
	} else if (os_strcmp(buf, "wpa_gmk_rekey") == 0) {
		bss->wpa_gmk_rekey = atoi(pos);
	} else if (os_strcmp(buf, "wpa_ptk_rekey") == 0) {
		bss->wpa_ptk_rekey = atoi(pos);
	} else if (os_strcmp(buf, "wpa_passphrase") == 0) {
		int len = os_strlen(pos);
		if (len < 8 || len > 63) {
			wpa_printf(MSG_ERROR, "Line %d: invalid WPA passphrase length %d (expected 8..63)",
				   line, len);
			return 1;
		}
		os_free(bss->ssid.wpa_passphrase);
		bss->ssid.wpa_passphrase = os_strdup(pos);
		if (bss->ssid.wpa_passphrase) {
			hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
			bss->ssid.wpa_passphrase_set = 1;
		}
	} else if (os_strcmp(buf, "wpa_psk") == 0) {
		hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
		bss->ssid.wpa_psk = os_zalloc(sizeof(struct hostapd_wpa_psk));
		if (bss->ssid.wpa_psk == NULL)
			return 1;
		if (hexstr2bin(pos, bss->ssid.wpa_psk->psk, PMK_LEN) ||
		    pos[PMK_LEN * 2] != '\0') {
			wpa_printf(MSG_ERROR, "Line %d: Invalid PSK '%s'.",
				   line, pos);
			hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
			return 1;
		}
		bss->ssid.wpa_psk->group = 1;
		os_free(bss->ssid.wpa_passphrase);
		bss->ssid.wpa_passphrase = NULL;
		bss->ssid.wpa_psk_set = 1;
	} else if (os_strcmp(buf, "wpa_key_mgmt") == 0) {
		bss->wpa_key_mgmt = hostapd_config_parse_key_mgmt(line, pos);
		if (bss->wpa_key_mgmt == -1)
			return 1;
	} else if (os_strcmp(buf, "wpa_pairwise") == 0) {
		bss->wpa_pairwise = hostapd_config_parse_cipher(line, pos);
		if (bss->wpa_pairwise == -1 || bss->wpa_pairwise == 0)
			return 1;
		if (bss->wpa_pairwise &
		    (WPA_CIPHER_NONE | WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)) {
			wpa_printf(MSG_ERROR, "Line %d: unsupported pairwise cipher suite '%s'",
				   bss->wpa_pairwise, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "rsn_pairwise") == 0) {
		bss->rsn_pairwise = hostapd_config_parse_cipher(line, pos);
		if (bss->rsn_pairwise == -1 || bss->rsn_pairwise == 0)
			return 1;
		if (bss->rsn_pairwise &
		    (WPA_CIPHER_NONE | WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)) {
			wpa_printf(MSG_ERROR, "Line %d: unsupported pairwise cipher suite '%s'",
				   bss->rsn_pairwise, pos);
			return 1;
		}
#ifdef CONFIG_PEERKEY
	} else if (os_strcmp(buf, "peerkey") == 0) {
		bss->peerkey = atoi(pos);
#endif /* CONFIG_PEERKEY */
	} else if (os_strcmp(buf, "use_pae_group_addr") == 0) {
		bss->use_pae_group_addr = atoi(pos);
	} else if (os_strcmp(buf, "hw_mode") == 0) {
		if (os_strcmp(pos, "a") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211A;
		else if (os_strcmp(pos, "b") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211B;
		else if (os_strcmp(pos, "g") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211G;
		else if (os_strcmp(pos, "ad") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211AD;
		else if (os_strcmp(pos, "any") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211ANY;
		else {
			wpa_printf(MSG_ERROR, "Line %d: unknown hw_mode '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "wps_rf_bands") == 0) {
		if (os_strcmp(pos, "ad") == 0)
			bss->wps_rf_bands = WPS_RF_60GHZ;
		else if (os_strcmp(pos, "a") == 0)
			bss->wps_rf_bands = WPS_RF_50GHZ;
		else if (os_strcmp(pos, "g") == 0 ||
			 os_strcmp(pos, "b") == 0)
			bss->wps_rf_bands = WPS_RF_24GHZ;
		else if (os_strcmp(pos, "ag") == 0 ||
			 os_strcmp(pos, "ga") == 0)
			bss->wps_rf_bands = WPS_RF_24GHZ | WPS_RF_50GHZ;
		else {
			wpa_printf(MSG_ERROR,
				   "Line %d: unknown wps_rf_band '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "channel") == 0) {
		conf->channel = atoi(pos);
		conf->acs = conf->channel == 0;
	} else if (os_strcmp(buf, "beacon_int") == 0) {
		int val = atoi(pos);
		/* MIB defines range as 1..65535, but very small values
		 * cause problems with the current implementation.
		 * Since it is unlikely that this small numbers are
		 * useful in real life scenarios, do not allow beacon
		 * period to be set below 15 TU. */
		if (val < 15 || val > 65535) {
			wpa_printf(MSG_ERROR, "Line %d: invalid beacon_int %d (expected 15..65535)",
				   line, val);
			return 1;
		}
		conf->beacon_int = val;
	} else if (os_strcmp(buf, "dtim_period") == 0) {
		bss->dtim_period = atoi(pos);
		if (bss->dtim_period < 1 || bss->dtim_period > 255) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dtim_period %d",
				   line, bss->dtim_period);
			return 1;
		}
	} else if (os_strcmp(buf, "supported_rates") == 0) {
		if (hostapd_parse_intlist(&conf->supported_rates, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rate list",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "basic_rates") == 0) {
		if (hostapd_parse_intlist(&conf->basic_rates, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rate list",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "ignore_broadcast_ssid") == 0) {
		bss->ignore_broadcast_ssid = atoi(pos);
	} else if (os_strcmp(buf, "wep_default_key") == 0) {
		bss->ssid.wep.idx = atoi(pos);
		if (bss->ssid.wep.idx > 3) {
			wpa_printf(MSG_ERROR,
				   "Invalid wep_default_key index %d",
				   bss->ssid.wep.idx);
			return 1;
		}
	} else if (os_strcmp(buf, "wep_key0") == 0 ||
		   os_strcmp(buf, "wep_key1") == 0 ||
		   os_strcmp(buf, "wep_key2") == 0 ||
		   os_strcmp(buf, "wep_key3") == 0) {
		if (hostapd_config_read_wep(&bss->ssid.wep,
					    buf[7] - '0', pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid WEP key '%s'",
				   line, buf);
			return 1;
		}
	} else if (os_strcmp(buf, "ap_table_max_size") == 0) {
		conf->ap_table_max_size = atoi(pos);
	} else if (os_strcmp(buf, "ap_table_expiration_time") == 0) {
		conf->ap_table_expiration_time = atoi(pos);
	} else if (os_strcmp(buf, "max_listen_interval") == 0) {
		bss->max_listen_interval = atoi(pos);
	} else if (os_strcmp(buf, "disable_pmksa_caching") == 0) {
		bss->disable_pmksa_caching = atoi(pos);
	} else if (os_strcmp(buf, "okc") == 0) {
		bss->okc = atoi(pos);
	} else if (os_strcmp(buf, "disassoc_low_ack") == 0) {
		bss->disassoc_low_ack = atoi(pos);
	} else if (os_strcmp(buf, "sae_anti_clogging_threshold") == 0) {
		bss->sae_anti_clogging_threshold = atoi(pos);
	} else if (os_strcmp(buf, "sae_groups") == 0) {
		if (hostapd_parse_intlist(&bss->sae_groups, pos)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid sae_groups value '%s'",
				   line, pos);
			return 1;
		}
	} else {
		wpa_printf(MSG_ERROR,
			   "Line %d: unknown configuration item '%s'",
			   line, buf);
		return 1;
	}

	return 0;
}


int hostapd_set_iface(struct hostapd_config *conf,
		      struct hostapd_bss_config *bss, const char *field,
		      char *value)
{
	int errors;
	size_t i;

	errors = hostapd_config_fill(conf, bss, field, value, 0);
	if (errors) {
		wpa_printf(MSG_INFO, "Failed to set configuration field '%s' "
			   "to value '%s'", field, value);
		return -1;
	}

	for (i = 0; i < conf->num_bss; i++)
		hostapd_set_security_params(conf->bss[i], 0);

	if (hostapd_config_check(conf, 0)) {
		wpa_printf(MSG_ERROR, "Configuration check failed");
		return -1;
	}

	return 0;
}
#endif /* CFG_NEW_SUPP */

/**
 * hostapd_driver_init - Preparate driver interface
 */
static int hostapd_driver_init(struct hostapd_iface *iface)
{
	size_t i;
	struct wpa_init_params params;
	struct hostapd_data *hapd = iface->bss[0];
	struct hostapd_bss_config *conf = hapd->conf;
	u8 *b = conf->bssid;
	struct wpa_driver_capa capa;

	if (hapd->driver == NULL || hapd->driver->hapd_init == NULL) {
		wpa_printf(MSG_ERROR, "No hostapd driver wrapper available");
		return -1;
	}

	/* Initialize the driver interface */
	if (is_zero_ether_addr(b)) {
		b = NULL;
        os_printf("hostapd_driver_init conf->bssid is null\r\n");
        return -1;
	}

	os_memset(&params, 0, sizeof(params));
	for (i = 0; wpa_drivers[i]; i++) {
		if (wpa_drivers[i] != hapd->driver)
			continue;

		if (s_hapd_global.drv_priv[i] == NULL &&
		    wpa_drivers[i]->global_init) {
			s_hapd_global.drv_priv[i] = wpa_drivers[i]->global_init();
			if (s_hapd_global.drv_priv[i] == NULL) {
				wpa_printf(MSG_ERROR, "Failed to initialize "
					   "driver '%s'",
					   wpa_drivers[i]->name);
				return -1;
			}
		}

		params.global_priv = s_hapd_global.drv_priv[i];
		break;
	}
	params.bssid = b;
	params.ifname = hapd->conf->iface;
	params.driver_params = hapd->iconf->driver_params;
	params.use_pae_group_addr = hapd->conf->use_pae_group_addr;

	params.num_bridge = hapd->iface->num_bss;
	params.bridge = os_calloc(hapd->iface->num_bss, sizeof(char *));
	if (params.bridge == NULL)
		return -1;

	for (i = 0; i < hapd->iface->num_bss; i++) {
		struct hostapd_data *bss = hapd->iface->bss[i];
#ifdef CONFIG_FULL_HOSTAPD
		if (bss->conf->bridge[0]) {
			params.bridge[i] = bss->conf->bridge;
		}
#endif
		os_printf("[csa]csa_in_progress[%d:%d]-clear\r\n", i, bss->csa_in_progress);
		bss->csa_in_progress = 0; /* test wangzhilei*/
	}

    os_memcpy(hapd->own_addr, b, ETH_ALEN);
	params.own_addr = hapd->own_addr;

	hapd->drv_priv = hapd->driver->hapd_init(hapd, &params);
	os_free(params.bridge);
	if (hapd->drv_priv == NULL) {
		wpa_printf(MSG_ERROR, "%s driver initialization failed.",
			   hapd->driver->name);
		hapd->driver = NULL;
		return -1;
	}

	if (hapd->driver->get_capa &&
	    hapd->driver->get_capa(hapd->drv_priv, &capa) == 0) {
		struct wowlan_triggers *triggs;

		iface->drv_flags = capa.flags;
		iface->smps_modes = capa.smps_modes;
		iface->probe_resp_offloads = capa.probe_resp_offloads;
		iface->extended_capa = capa.extended_capa;
		iface->extended_capa_mask = capa.extended_capa_mask;
		iface->extended_capa_len = capa.extended_capa_len;
		iface->drv_max_acl_mac_addrs = capa.max_acl_mac_addrs;
#ifdef CONFIG_FULL_HOSTAPD
		triggs = wpa_get_wowlan_triggers(conf->wowlan_triggers, &capa);
		if (triggs && hapd->driver->set_wowlan) {
			if (hapd->driver->set_wowlan(hapd->drv_priv, triggs))
				wpa_printf(MSG_ERROR, "set_wowlan failed");
		}
		os_free(triggs);
#endif
	}

	return 0;
}


/**
 * hostapd_interface_init - Read configuration file and init BSS data
 *
 * This function is used to parse configuration file for a full interface (one
 * or more BSSes sharing the same radio) and allocate memory for the BSS
 * g_hapd_interfaces. No actiual driver operations are started.
 */
static struct hostapd_iface *
hostapd_interface_init(struct hapd_interfaces *interfaces,
		       const char *config_fname, int debug)
{
	struct hostapd_iface *iface;
	int k;

	wpa_printf(MSG_ERROR, "Configuration file: %s", config_fname);
	iface = hostapd_init(interfaces, config_fname);
	if (!iface)
		return NULL;
	iface->interfaces = interfaces;

	for (k = 0; k < debug; k++) {
		if (iface->bss[0]->conf->logger_stdout_level > 0)
			iface->bss[0]->conf->logger_stdout_level--;
	}

	if (iface->conf->bss[0]->iface[0] == '\0') {
		wpa_printf(MSG_ERROR, "Interface name not specified in %s",
			   config_fname);
		hostapd_interface_deinit_free(iface);
		return NULL;
	}

	return iface;
}

static int hostapd_global_init(struct hapd_interfaces *interfaces,
			       const char *entropy_file)
{
	int i;

	os_memset(&s_hapd_global, 0, sizeof(s_hapd_global));

	for (i = 0; wpa_drivers[i]; i++)
		s_hapd_global.drv_count++;
	if (s_hapd_global.drv_count == 0) {
		wpa_printf(MSG_ERROR, "No drivers enabled");
		return -1;
	}
	s_hapd_global.drv_priv = os_calloc(s_hapd_global.drv_count, sizeof(void *));
	if (s_hapd_global.drv_priv == NULL)
		return -1;

	return 0;
}


static void hostapd_global_deinit(const char *pid_file)
{
	int i;

	for (i = 0; wpa_drivers[i] && s_hapd_global.drv_priv; i++) {
		if (!s_hapd_global.drv_priv[i])
			continue;
		wpa_drivers[i]->global_deinit(s_hapd_global.drv_priv[i]);
	}
	os_free(s_hapd_global.drv_priv);
	s_hapd_global.drv_priv = NULL;

	eloop_free_resource();

	os_daemonize_terminate(pid_file);
}

static int hostapd_global_run(struct hapd_interfaces *ifaces, int daemonize,
			      const char *pid_file)
{
	if (daemonize && os_daemonize(pid_file)) {
		return -1;
	}

	eloop_run();

	return 0;
}

const char * hostapd_msg_ifname_cb(void *ctx)
{
	struct hostapd_data *hapd = ctx;
	if (hapd && hapd->iconf && hapd->iconf->bss &&
	    hapd->iconf->num_bss > 0 && hapd->iconf->bss[0])
		return hapd->iconf->bss[0]->iface;
	return NULL;
}

/* Periodic cleanup tasks */
static void hostapd_periodic(void *eloop_ctx, void *timeout_ctx)
{
}

int hostapd_main_exit(void)
{
	size_t i;

	if (0 == g_hapd_interfaces.count)
		return 0;

	for (i = 0; i < g_hapd_interfaces.count; i++) {
		if (!g_hapd_interfaces.iface[i])
			continue;

		g_hapd_interfaces.iface[i]->driver_ap_teardown =
			!!(g_hapd_interfaces.iface[i]->drv_flags &
			   WPA_DRIVER_FLAGS_AP_TEARDOWN_SUPPORT);

		hostapd_interface_deinit_free(g_hapd_interfaces.iface[i]);
		g_hapd_interfaces.iface[i] = NULL;
	}
	os_free(g_hapd_interfaces.iface);
	g_hapd_interfaces.iface = NULL;
	g_hapd_interfaces.count = 0;

	eloop_cancel_timeout(hostapd_periodic, &g_hapd_interfaces, NULL);
	hostapd_global_deinit(NULL);

	return 0;
}

/**
 * Usage:
 *   hostapd -g /var/run/hostapd/global -b phy0:ath10k.conf -b phy0:ath10k-1.conf -b phy0:ath10k-2.conf
 *   -b <phyX>:<bss.conf> : add a bss config
 *
 *   ath10k-1.conf:
 *       interface=wlan0-1
 *       ssid=ath10k-1
 *       bssid=02:00:00:00:03:01
 *
 *   ath10k-2.conf:
 *       interface=wlan0-2
 *       ssid=ath10k-2
 *       bssid=02:00:00:00:03:02
 *       wpa=2
 *       wpa_passphrase=0987654321
 *       wpa_key_mgmt=WPA-PSK
 *       rsn_pairwise=CCMP
 *
 */
int hostapd_main_entry(int argc, char *argv[])
{
	int ret = 1;
	size_t i, j;
	int debug = 0;
	char *pid_file = NULL;
	size_t num_bss_configs = 0;
	const char *log_file = NULL;
	const char *entropy_file = NULL;
	char *bss_config[1] = {CFG_BSS_CONFIG};		//"wangzhilei_config:bss_fname"
	//char *ap_iface_buf = CFG_AP_IFACE_CONFIG;	//"bss_config= wlan0"

	//ap_iface_buf = os_zalloc(strlen(CFG_AP_IFACE_CONFIG) + 2);
	//if (0 == ap_iface_buf)
	//	return -1;

	//os_memcpy(ap_iface_buf, CFG_AP_IFACE_CONFIG,(strlen(CFG_AP_IFACE_CONFIG) + 1));
	if (os_program_init()) {
		//os_free(ap_iface_buf);
		return -1;
	}

	os_memset(&g_hapd_interfaces, 0, sizeof(g_hapd_interfaces));
	g_hapd_interfaces.reload_config  = hostapd_reload_config;
	g_hapd_interfaces.config_read_cb = hostapd_config_read;
	g_hapd_interfaces.for_each_interface = hostapd_for_each_interface;
	g_hapd_interfaces.ctrl_iface_init    = 0;
	g_hapd_interfaces.ctrl_iface_deinit  = 0;
	g_hapd_interfaces.driver_init = hostapd_driver_init;
	g_hapd_interfaces.global_iface_path = NULL;
	g_hapd_interfaces.global_iface_name = NULL;
	g_hapd_interfaces.global_ctrl_sock  = -1;
	g_hapd_interfaces.global_ctrl_dst   = NULL;

	wpa_msg_register_ifname_cb(hostapd_msg_ifname_cb);

	g_hapd_interfaces.count = argc - 1;
	if (g_hapd_interfaces.count || num_bss_configs) {
		g_hapd_interfaces.iface = os_calloc(g_hapd_interfaces.count + num_bss_configs,
											sizeof(struct hostapd_iface *));
		if (g_hapd_interfaces.iface == NULL) {
			//os_free(ap_iface_buf);
			fatal_prf("malloc failed\r\n");
			return -1;
		}
	}

	if (hostapd_global_init(&g_hapd_interfaces, entropy_file)) {
		//os_free(ap_iface_buf);
		fatal_prf("Failed to initialize global context\r\n");
		return -1;
	}

	/* Allocate and parse configuration for full interface files */
	for (i = 0; i < g_hapd_interfaces.count; i++) {
		char *config_fname = CFG_CONFIG_FNAME;	//"beken_cfg_fname"
		g_hapd_interfaces.iface[i] = hostapd_interface_init(&g_hapd_interfaces,
									 config_fname,
									 debug);
		if (!g_hapd_interfaces.iface[i]) {
			fatal_prf("Failed to initialize interface\r\n");
			goto out;
		}
	}

#ifdef CONFIG_MBSSID
	/* Allocate and parse configuration for per-BSS files */
	for (i = 0; i < num_bss_configs; i++) {
		struct hostapd_iface *iface;
		char *fname;

		wpa_printf(MSG_INFO, "BSS config: %s", bss_config[i]);
		fname = os_strchr(bss_config[i], ':');
		if (fname == NULL) {
			wpa_printf(MSG_ERROR,
					   "Invalid BSS config identifier '%s'",
					   bss_config[i]);
			goto out;
		}
		*fname++ = '\0';
		iface = hostapd_interface_init_bss(&g_hapd_interfaces, bss_config[i],
										   fname, debug);
		if (iface == NULL)
			goto out;
		for (j = 0; j < g_hapd_interfaces.count; j++) {
			if (g_hapd_interfaces.iface[j] == iface)
				break;
		}
		if (j == g_hapd_interfaces.count) {
			struct hostapd_iface **tmp;
			tmp = os_realloc_array(g_hapd_interfaces.iface,
								   g_hapd_interfaces.count + 1,
								   sizeof(struct hostapd_iface *));
			if (tmp == NULL) {
				hostapd_interface_deinit_free(iface);
				goto out;
			}
			g_hapd_interfaces.iface = tmp;
			g_hapd_interfaces.iface[g_hapd_interfaces.count++] = iface;
		}
	}
#endif

	/*
	 * Enable configured g_hapd_interfaces. Depending on channel configuration,
	 * this may complete full initialization before returning or use a
	 * callback mechanism to complete setup in case of operations like HT
	 * co-ex scans, ACS, or DFS are needed to determine channel parameters.
	 * In such case, the interface will be enabled from eloop context within
	 * hostapd_global_run().
	 */
	g_hapd_interfaces.terminate_on_error = g_hapd_interfaces.count;
	for (i = 0; i < g_hapd_interfaces.count; i++) {
		if (hostapd_driver_init(g_hapd_interfaces.iface[i]) ||
			hostapd_setup_interface(g_hapd_interfaces.iface[i]))
			goto out;
	}

#if 0//def NOT_USED
	hostapd_add_iface(&g_hapd_interfaces, ap_iface_buf);	//ap_iface_buf: "bss_config= wlan0"
#endif

	ret = 0;

	//os_free(ap_iface_buf);
	//ap_iface_buf = NULL;

	return ret;

out:
	/* Deinitialize all g_hapd_interfaces */
	fatal_prf("hostapd_main_init_failed\r\n");
	for (i = 0; i < g_hapd_interfaces.count; i++) {
		if (!g_hapd_interfaces.iface[i])
			continue;

		g_hapd_interfaces.iface[i]->driver_ap_teardown =
			!!(g_hapd_interfaces.iface[i]->drv_flags &
			   WPA_DRIVER_FLAGS_AP_TEARDOWN_SUPPORT);

		hostapd_interface_deinit_free(g_hapd_interfaces.iface[i]);
		g_hapd_interfaces.iface[i] = NULL;
	}
	os_free(g_hapd_interfaces.iface);
	g_hapd_interfaces.iface = NULL;
	g_hapd_interfaces.count = 0;

	eloop_cancel_timeout(hostapd_periodic, &g_hapd_interfaces, NULL);
	hostapd_global_deinit(pid_file);

	os_free(pid_file);
	pid_file = NULL;

	//os_free(ap_iface_buf);
	//ap_iface_buf = NULL;

	if (log_file)
		wpa_debug_close_file();
	wpa_debug_close_linux_tracing();

	os_program_deinit();

	return ret;
}


static void hostapd_thread_main( void *arg )
{
	int daemonize = 0;
    char *pid_file = NULL;

    if (hostapd_global_run(&g_hapd_interfaces, daemonize, pid_file)) {
        wpa_printf(MSG_ERROR, "Failed to start eloop");
    }
}

#if 0
void hostapd_thread_start(void)
{
    OSStatus ret;

    if(wpah_queue == NULL) {
    	ret = rtos_init_queue(&wpah_queue,
    							"wpah_queue",
    							sizeof(wpah_msg_t),
    							64);
        ASSERT(kNoErr == ret);
    }

    if((hostapd_thread_handle== NULL) && (NULL == wpas_thread_handle)) {
        ret = rtos_create_thread(&hostapd_thread_handle,
                THD_HOSTAPD_PRIORITY,
                "hostapd_thread",
                (beken_thread_function_t)hostapd_thread_main,
                (unsigned short)hostapd_stack_size,
                (beken_thread_arg_t)NULLPTR);
        ASSERT(kNoErr == ret);
    }
}

void hostapd_thread_stop(void)
{
    OSStatus ret;

    ret = rtos_delete_thread(&hostapd_thread_handle);
    ASSERT(kNoErr == ret);

    ret = rtos_deinit_semaphore(&hostapd_sema);
    ASSERT(kNoErr == ret);
}

int hostapd_sem_wait(uint32_t ms)
{
	if (NULL == hostapd_sema)
		return kTimeoutErr;

	return rtos_get_semaphore(&hostapd_sema, ms);
}

void hostapd_poll(void *param)
{
	OSStatus ret;

	if (hostapd_sema) {
		ret = rtos_set_semaphore(&hostapd_sema);
		ASSERT(kNoErr == ret);
	}
}
#endif

int hostapd_channel_switch(int new_freq)
{
    return ap_channel_switch(g_hapd_interfaces.iface[0], new_freq);
}

#if CFG_NEW_SUPP
int wpa_hostapd_queue_command(wpah_msg_t *msg)
{
	int ret = -1;

	if (!wpah_queue)
		goto poll_exit;

	ret = rtos_push_to_queue(&wpah_queue, msg, BEKEN_NO_WAIT);
	if (kNoErr != ret)
		os_printf("wpa_hostapd_queue_command:%d\r\n", ret);

poll_exit:
	return ret;
}
#endif

uint32_t wpa_hostapd_queue_poll(uint32_t param)
{
	OSStatus ret = 0;
	wpah_msg_t msg;

	if (NULL == wpah_queue)
		goto poll_exit;

#if CFG_NEW_SUPP
	msg.cmd = WPA_CTRL_CMD_SOCKET;;
#endif
	msg.argu = (u32)param;
	ret = rtos_push_to_queue(&wpah_queue, &msg, BEKEN_NO_WAIT);
	if (kNoErr != ret)
		os_printf("wpa_hostapd_queue_poll_failed:%d\r\n", ret);

poll_exit:
	return ret;
}

// eof

