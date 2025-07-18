#include <linux/module.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/of_gpio.h>
#include <linux/power_supply.h>
//#include <linux/hardware_info.h>
#include "battery_auth_class.h"

extern uint8_t auth_get_batt_id(void);
extern int ds28e30_get_page_data_retry(int page, unsigned char *data);
extern int ds28e30_Read_RomID_retry(unsigned char *RomID);

enum {
	MAIN_SUPPLY = 0,
	SECEON_SUPPLY,
	THIRD_SUPPLY,
	MAX_SUPPLY,
};


static const char *auth_device_name[] = {
	"main_supplier",
	"second_supplier",
	"third_supplier",
	"unknown",
};


struct auth_data {
	struct auth_device *auth_dev[MAX_SUPPLY];

	struct power_supply *verify_psy;
	struct power_supply_desc desc;

	struct delayed_work dwork;

	bool auth_result;
	u8 batt_id;
	u8 *soh_sn;
	int first_use_date;
	int asoc;
	int bsoh;
	int bsoh_raw;
	int fai_expired;
	int batt_discharge_level;
	int batt_full_status_usage;
	int sync_buf_mem_sts;
	int sync_buf_mem;
	bool asocflg;
	bool bsohflg;
	bool batt_discharge_level_flg;
};

static struct auth_data *g_info;
static int auth_index = 0;

void auth_get_batt_discharge_level(int cycle)
{
	if (g_info != NULL) {
		if(!g_info->batt_discharge_level_flg)
			g_info->batt_discharge_level = cycle;
	}
}
EXPORT_SYMBOL(auth_get_batt_discharge_level);

int batt_auth_get_cycle_count(void)
{
	int count = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		count = auth_device_get_cycle_count(g_info->auth_dev[auth_index]);
		pr_err("%s index:%d, count:%d\n", __func__, auth_index, count);
		return count;
	} else
		return -1;
}
EXPORT_SYMBOL(batt_auth_get_cycle_count);

int batt_auth_set_cycle_count(int count, int get_count)
{
	int ret = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_cycle_count(g_info->auth_dev[auth_index], count, get_count);
		pr_err("%s index:%d, count:%d, get_count:%d, ret:%d\n",
			__func__, auth_index, count, get_count, ret);
		if (ret < 0)
			return -1;
		else
			return count;
	} else
		return -1;
}
EXPORT_SYMBOL(batt_auth_set_cycle_count);

static ssize_t batt_discharge_level_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		g_info->batt_discharge_level = auth_device_get_batt_discharge_level(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, g_info->batt_discharge_level);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->batt_discharge_level = 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->batt_discharge_level);
}

static ssize_t batt_discharge_level_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->batt_discharge_level) != 1)
		return -EINVAL;

	g_info->batt_discharge_level_flg = true;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_batt_discharge_level(g_info->auth_dev[auth_index], g_info->batt_discharge_level);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->batt_discharge_level, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->batt_discharge_level = 0;
	}

	return count;
}

static DEVICE_ATTR_RW(batt_discharge_level);

static ssize_t batt_full_status_usage_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		g_info->batt_full_status_usage = auth_device_get_batt_full_status_usage(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, g_info->batt_full_status_usage);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->batt_full_status_usage = 0;
	}
	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->batt_full_status_usage);
}

static ssize_t batt_full_status_usage_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->batt_full_status_usage) != 1)
		return -EINVAL;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_batt_full_status_usage(g_info->auth_dev[auth_index], g_info->batt_full_status_usage);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->batt_full_status_usage, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->batt_full_status_usage = 0;
	}

	return count;
}

static DEVICE_ATTR_RW(batt_full_status_usage);


static enum power_supply_property verify_props[] = {
	POWER_SUPPLY_PROP_AUTHENTIC,
	POWER_SUPPLY_PROP_TYPE,
	POWER_SUPPLY_PROP_MANUFACTURER,
};

static int verify_get_property(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	static bool first_flg = true;
	int i = 0;
	int authen_result;

	pr_info("%s:%d\n", __func__, psp);
	if (auth_get_batt_id() != 0 && first_flg) {
		first_flg = false;
		for (i = 0; i < MAX_SUPPLY; i++) {
			g_info->auth_dev[i] = get_batt_auth_by_name(auth_device_name[i]);
			if (!g_info->auth_dev[i]) {
				pr_info("get_batt_auth_by_name[%d]\n", i);
				break;
			}
		}
	} else if (auth_get_batt_id() == 0)
		return 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_AUTHENTIC:
		for (i = 0; i < MAX_SUPPLY; i++) {
			if (!g_info->auth_dev[i]) {
				pr_info("g_info->auth_dev[%d]=\n", i);
				continue;
			}
			authen_result = auth_device_start_auth(g_info->auth_dev[i]);
			pr_info("authentic result is :%d,i=%d\n", authen_result,i);
			if (!authen_result) {
				auth_device_get_batt_id(g_info->auth_dev[i], &(g_info->batt_id));
				auth_index = i;
				break;
			}
		}
		g_info->auth_result = ((authen_result == 0) ? true : false);
		val->intval = g_info->auth_result;
		pr_info("%s:auth_result:%d\n", __func__, g_info->auth_result);
		break;
	case POWER_SUPPLY_PROP_TYPE:
		val->intval = g_info->batt_id;
		pr_info("%s:batt_id:%d\n", __func__, g_info->batt_id);
		break;
	case POWER_SUPPLY_PROP_MANUFACTURER:
		val->strval = auth_device_name[auth_index];
		pr_info("%s:auth_index:%d,name:%s\n", __func__, auth_index,val->strval);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

#define AUTHENTIC_COUNT_MAX 3
static void auth_battery_dwork(struct work_struct *work)
{
	int i = 0;
	struct auth_data *info = container_of(to_delayed_work(work),
					      struct auth_data, dwork);
	static bool first_flg = true;

	int authen_result;
	static int retry_authentic = 0;

	if(first_flg) {
		first_flg = false;
		for (i = 0; i < MAX_SUPPLY; i++) {
			info->auth_dev[i] = get_batt_auth_by_name(auth_device_name[i]);
			if (!info->auth_dev[i]) {
				pr_info("get_batt_auth_by_name[%d]=\n", i);
				break;
			}
		}
	}
	for (i = 0; i < MAX_SUPPLY; i++) {
		if (!info->auth_dev[i]) {
			pr_info("info->auth_dev[%d]=\n", i);
			continue;
		}
		authen_result = auth_device_start_auth(info->auth_dev[i]);
		pr_info("authentic result is :%d,i=%d\n", authen_result,i);
		if (!authen_result) {
			auth_device_get_batt_id(info->auth_dev[i], &(info->batt_id));
			auth_index = i;
			/*if (info->batt_id != 0xff) {
				info->soh_sn = NULL;
				auth_device_get_batt_sn(info->auth_dev[i], info->soh_sn);
			}*/
			break;
		}
	}

	if (info->batt_id == 0xff) {
		retry_authentic++;
		if (retry_authentic < AUTHENTIC_COUNT_MAX) {
			pr_info
			    ("battery authentic work begin to restart %d\n",
			     retry_authentic);
			schedule_delayed_work(&(info->dwork),
					      msecs_to_jiffies(5000));
		}
		if (retry_authentic == AUTHENTIC_COUNT_MAX) {
			pr_info("authentic result is %s\n",
				(authen_result == 0) ? "success" : "fail");
			info->batt_id = 0xff;
			retry_authentic = 0;
		}
	} else
		pr_info("authentic result is %s, batt_id:%d\n",
			(authen_result == 0) ? "success" : "fail", info->batt_id);

	info->auth_result = ((authen_result == 0) ? true : false);
#if 0
	switch (info->batt_id) {
		case 1:
			hardwareinfo_set_prop(HARDWARE_BATTERY_ID, "P86801_SCUD_LI-ION_4v4_7040mah");
			break;
		case 2:
			hardwareinfo_set_prop(HARDWARE_BATTERY_ID, "P86801_Ningde_LI-ION_4v4_7040mah");
			break;
		case 3:
			hardwareinfo_set_prop(HARDWARE_BATTERY_ID, "P86801_TBD_LI-ION_4v4_7040mah");
			break;
		default:
			hardwareinfo_set_prop(HARDWARE_BATTERY_ID, "Unknow");
			break;
	}
#endif
}

static void check_remove_plus_char(unsigned char *c, unsigned char *temp) {
	int i = 0;

	for (i = 0; (i < 32) && (*c != '\0'); i++) {
		if (*c != '+')
			*temp++ = *c++;
		else
			c++;
	}
}
static ssize_t qr_code_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret;
	unsigned char page0_data[50];
	unsigned char page0_temp_data[50];

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	ret = ds28e30_get_page_data_retry(0, page0_data);
	check_remove_plus_char(page0_data, page0_temp_data);
	g_info->soh_sn = page0_temp_data;
	pr_info("%s:%s,ret=%d\n", __func__, g_info->soh_sn, ret);
	return scnprintf(buf, PAGE_SIZE, "%s\n", g_info->soh_sn);
	if (ret != 1)
		return -EAGAIN;
}

static DEVICE_ATTR_RO(qr_code);

static ssize_t presence_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	unsigned int presence = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (g_info->auth_dev[auth_index])
			auth_device_get_batt_id(g_info->auth_dev[auth_index], &(g_info->batt_id));

	if (g_info->batt_id != 0)
		presence = 1;
	else
		presence = 0;
	pr_info("%s:%d\n", __func__, presence);
	return scnprintf(buf, PAGE_SIZE, "%d\n", presence);
}

static DEVICE_ATTR_RO(presence);

static ssize_t batt_auth_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int i = 0;
	int authen_result;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	for (i = 0; i < MAX_SUPPLY; i++) {
		if (!g_info->auth_dev[i]) {
			pr_info("g_info->auth_dev[%d]=\n", i);
			continue;
		}
		authen_result = auth_device_start_auth(g_info->auth_dev[i]);
		pr_info("authentic result is :%d,i=%d\n", authen_result,i);
		if (!authen_result)
			break;
	}
	g_info->auth_result = ((authen_result == 0) ? true : false);
	pr_info("%s:%d\n", __func__, g_info->auth_result);
	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->auth_result);
}

static DEVICE_ATTR_RO(batt_auth);

static ssize_t fai_expired_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	pr_info("%s:%d\n", __func__, g_info->fai_expired);
	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->fai_expired);
}

static ssize_t  fai_expired_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->fai_expired) != 1)
		return -EINVAL;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_fai_expired(g_info->auth_dev[auth_index], g_info->fai_expired);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->fai_expired, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->fai_expired = 0;
	}

	return count;
}

static DEVICE_ATTR_RW(fai_expired);

static ssize_t first_use_date_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		g_info->first_use_date = auth_device_get_first_use_date(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, g_info->first_use_date);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->first_use_date = 0xFF;
	}
	if (g_info->first_use_date == 0xFF)
		return scnprintf(buf, PAGE_SIZE, "%X\n", g_info->first_use_date);
	else
		return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->first_use_date);
}

static ssize_t first_use_date_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if ((strncmp(buf, "FF", 2) == 0) || (strncmp(buf, "ff", 2) == 0)) {
		pr_info("%s:%s,FF!!!\n", __func__, buf);
		if (sscanf(buf, "%x", &g_info->first_use_date) != 1)
			return -EINVAL;
	} else {
		pr_info("%s:%s\n", __func__, buf);
		if (sscanf(buf, "%d", &g_info->first_use_date) != 1)
			return -EINVAL;
	}

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_first_use_date(g_info->auth_dev[auth_index], g_info->first_use_date);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->first_use_date, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->first_use_date = 0xFF;
	}

	return count;
}

static DEVICE_ATTR_RW(first_use_date);

void auth_get_soc(int asoc)
{
	if (g_info != NULL) {
		if(!g_info->asocflg)
			g_info->asoc = asoc;
	}
}
EXPORT_SYMBOL(auth_get_soc);

static ssize_t asoc_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	pr_info("%s:%d\n", __func__, g_info->asoc);
	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->asoc);
}

static ssize_t asoc_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->asoc) != 1)
		return -EINVAL;

	g_info->asocflg = true;
	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_batt_asoc(g_info->auth_dev[auth_index], g_info->asoc);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->asoc, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		return -1;
	}

	return count;
}

static DEVICE_ATTR_RW(asoc);

static ssize_t sync_buf_mem_sts_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int sync_buf_mem_sts = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		sync_buf_mem_sts = auth_device_get_sync_buf_mem_sts(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, sync_buf_mem_sts);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
	}
	if(sync_buf_mem_sts != 0)
		g_info->sync_buf_mem_sts = sync_buf_mem_sts;

	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->sync_buf_mem_sts);
}

static ssize_t  sync_buf_mem_sts_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->sync_buf_mem_sts) != 1)
		return -EINVAL;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_sync_buf_mem_sts(g_info->auth_dev[auth_index], g_info->sync_buf_mem_sts);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->sync_buf_mem_sts, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->sync_buf_mem_sts = 1;
	}

	return count;
}

static DEVICE_ATTR_RW(sync_buf_mem_sts);

static ssize_t sync_buf_mem_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int sync_buf_mem = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		sync_buf_mem = auth_device_get_sync_buf_mem(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, sync_buf_mem);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
	}
	if(sync_buf_mem != 0)
		g_info->sync_buf_mem = sync_buf_mem;

	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->sync_buf_mem);
}

static ssize_t  sync_buf_mem_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->sync_buf_mem) != 1)
		return -EINVAL;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_sync_buf_mem(g_info->auth_dev[auth_index], g_info->sync_buf_mem);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->sync_buf_mem, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		g_info->sync_buf_mem = 1;
	}

	return count;
}

static DEVICE_ATTR_RW(sync_buf_mem);

static ssize_t chipname_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret;
	unsigned char mi_romid[8] = { 0x00 };

	ret = ds28e30_Read_RomID_retry(mi_romid);
	pr_info("%s:%d\n", __func__, ret);
	if (ret == true)
		return scnprintf(buf, PAGE_SIZE, "ds28e30\n");
	else
		return scnprintf(buf, PAGE_SIZE, "unknown\n");
}

static DEVICE_ATTR_RO(chipname);

void auth_get_bsoh(int bsoh)
{
	if (g_info != NULL) {
		if(!g_info->bsohflg)
			g_info->bsoh = bsoh * 100;
	}
}
EXPORT_SYMBOL(auth_get_bsoh);

static ssize_t bsoh_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->bsoh) != 1)
		return -EINVAL;

	g_info->bsohflg = true;
	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_batt_bsoh(g_info->auth_dev[auth_index], g_info->bsoh);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->bsoh, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		return -1;
	}

	return count;
}

static ssize_t bsoh_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int bsoh = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		bsoh = auth_device_get_batt_bsoh(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, bsoh);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
	}
	g_info->bsoh = bsoh;

	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->bsoh);
}

static DEVICE_ATTR_RW(bsoh);

static ssize_t bsoh_raw_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	int ret = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}

	if (sscanf(buf, "%d", &g_info->bsoh_raw) != 1)
		return -EINVAL;

	if (g_info->auth_dev[auth_index]) {
		ret = auth_device_set_batt_bsoh_raw(g_info->auth_dev[auth_index], g_info->bsoh_raw);
		pr_info("%s:%d,ret=%d\n", __func__, g_info->bsoh_raw, ret);
		if (ret < 0)
			return -1;
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
		return -1;
	}

	return count;
}

static ssize_t bsoh_raw_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	int bsoh_raw = 0;

	if (!g_info) {
		pr_err("%s g_info is null, fail\n", __func__);
		return -1;
	}
	if (g_info->auth_dev[auth_index]) {
		bsoh_raw = auth_device_get_batt_bsoh_raw(g_info->auth_dev[auth_index]);
		pr_info("%s:%d\n", __func__, bsoh_raw);
	} else {
		pr_err("%s g_info->auth_dev[auth_index] is null, fail\n", __func__);
	}
	g_info->bsoh_raw = bsoh_raw;

	return scnprintf(buf, PAGE_SIZE, "%d\n", g_info->bsoh_raw);
}

static DEVICE_ATTR_RW(bsoh_raw);


static int get_auth_board_id(void) {
	char *bootmode_string = NULL;
	char bootmode_start[32] = " ";
	int rc;

#ifdef CONFIG_QGKI_BUILD
	bootmode_string = strstr(saved_command_line,"board_id=");
#else
	bootmode_string="board_id=P86801AA1";
#endif /* CONFIG_QGKI_BUILD */
	if(NULL != bootmode_string){
		strncpy(bootmode_start, bootmode_string+9, 9);
		pr_info("%s: %s\n", __func__, bootmode_start);
		rc = strncmp(bootmode_start, "P86801JA1", 9);
		if(0 == rc){
			return 1;
		}
		rc = strncmp(bootmode_start, "P86803DA1", 9);
		if(0 == rc){
			return 1;
		}
	}
	pr_info("%s fail!\n", __func__);
	return 0;
}

static int __init auth_battery_init(void)
{
	struct auth_data *info;
	struct power_supply_config cfg = { };
	int ret = 0;

	pr_info("%s enter\n", __func__);
	if (get_auth_board_id() == 0)
		return 0;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	cfg.drv_data = info;
	info->desc.name = "sec_auth";
	info->desc.type = POWER_SUPPLY_TYPE_BATTERY;
	info->desc.properties = verify_props;
	info->desc.num_properties = ARRAY_SIZE(verify_props);
	info->desc.get_property = verify_get_property;
	info->verify_psy =
	    power_supply_register(NULL, &(info->desc), &cfg);
	if (!(info->verify_psy)) {
		pr_err("%s register verify psy fail\n", __func__);
	}
	info->first_use_date = 0xFF;
	info->asoc = 100;
	info->bsoh = 10000;
	info->bsoh_raw = 10000;
	info->fai_expired = 0;
	info->batt_discharge_level = 0;
	info->batt_full_status_usage = 0;
	info->sync_buf_mem_sts = 1;
	info->sync_buf_mem = 1;
	info->asocflg = false;
	info->bsohflg = false;
	info->batt_discharge_level_flg = false;

	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_qr_code);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_presence);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_batt_auth);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_chipname);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_fai_expired);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_first_use_date);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_batt_discharge_level);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_batt_full_status_usage);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_asoc);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_sync_buf_mem_sts);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_sync_buf_mem);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_bsoh);
	ret = device_create_file(&(info->verify_psy->dev), &dev_attr_bsoh_raw);

	INIT_DELAYED_WORK(&info->dwork, auth_battery_dwork);
	g_info = info;
	//schedule_delayed_work(&info->dwork, msecs_to_jiffies(2000));

	return 0;
}

static void __exit auth_battery_exit(void)
{
	int i = 0;

	power_supply_unregister(g_info->verify_psy);

	for (i = 0; i < MAX_SUPPLY; i++)
		auth_device_unregister(g_info->auth_dev[i]);

	kfree(g_info);
}

module_init(auth_battery_init);
module_exit(auth_battery_exit);
MODULE_LICENSE("GPL");
