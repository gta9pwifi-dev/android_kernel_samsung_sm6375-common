#ifndef __BATT_AUTH_CLASS__
#define __BATT_AUTH_CLASS__

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/of_gpio.h>

struct auth_device {
	const char *name;
	const struct auth_ops *ops;
	raw_spinlock_t io_lock;
	struct device dev;
	void *drv_data;
	//struct gpio_desc *gpiod;
	//struct platform_device *pdev;
	uint16_t gpio;
	int authon_data_gpio;
};

#define to_auth_device(obj) container_of(obj, struct auth_device, dev)

//battery pagedata
#define SCUD            0x46
#define BYD             0x44
#define QIANFENG        0x46
//#define NVT_SN1         0x4e
//#define NVT_SN2         0x56
#define SWD_SN1         0x53
#define SWD_SN2         0x4C
#define GY_SN1          0x47
#define GY_SN2          0x45

//battery supply
#define BATTERY_VENDOR_FIRST      1
#define BATTERY_VENDOR_SECOND     2
#define BATTERY_VENDOR_THIRD      3
#define BATTERY_VENDOR_UNKNOW    0xff

#define DTS_VOlT_REGULATER "vfp"

static char* battery_name_txt[] = {
        [BATTERY_VENDOR_FIRST] = "P86801_SCUD_LI-ION_4v4_7040mah",
        [BATTERY_VENDOR_SECOND] = "P86801_Ningde_LI-ION_4v4_7040mah",
        [BATTERY_VENDOR_THIRD] = "P86801_TBD_LI-ION_4v4_7040mah",
        [BATTERY_VENDOR_UNKNOW] = "UNKNOWN",
};

struct auth_ops {
	int (*auth_battery) (struct auth_device * auth_dev);
	int (*get_battery_id) (struct auth_device * auth_dev, u8 * id);
	int (*get_batt_sn) (struct auth_device *auth_dev, u8 *soh_sn);
	int (*get_ui_soh) (struct auth_device *auth_dev, u8 *ui_soh_data, int len);
	int (*set_ui_soh) (struct auth_device *auth_dev, u8 *ui_soh_data, int len, int raw_soh);
	int (*get_cycle_count) (struct auth_device * auth_dev);
	int (*set_cycle_count) (struct auth_device * auth_dev, int count, int get_count);
	int (*get_first_use_date) (struct auth_device * auth_dev);
	int (*set_first_use_date) (struct auth_device * auth_dev, int first_use_date);
	int (*get_batt_discharge_level) (struct auth_device * auth_dev);
	int (*set_batt_discharge_level) (struct auth_device * auth_dev, int batt_discharge_level);
	int (*get_batt_bsoh) (struct auth_device * auth_dev);
	int (*set_batt_bsoh) (struct auth_device * auth_dev, int bsoh);
	int (*get_batt_bsoh_raw) (struct auth_device * auth_dev);
	int (*set_batt_bsoh_raw) (struct auth_device * auth_dev, int bsoh_raw);
	int (*get_batt_full_status_usage) (struct auth_device * auth_dev);
	int (*set_batt_full_status_usage) (struct auth_device * auth_dev, int batt_full_status_usage);
	int (*get_batt_asoc) (struct auth_device * auth_dev);
	int (*set_batt_asoc) (struct auth_device * auth_dev, int asoc);
	int (*get_fai_expired) (struct auth_device * auth_dev);
	int (*set_fai_expired) (struct auth_device * auth_dev, int asoc);
	int (*get_sync_buf_mem_sts) (struct auth_device * auth_dev);
	int (*set_sync_buf_mem_sts) (struct auth_device * auth_dev, int sync_buf_mem_sts);
	int (*get_sync_buf_mem) (struct auth_device * auth_dev);
	int (*set_sync_buf_mem) (struct auth_device * auth_dev, int sync_buf_mem);
};

int auth_device_start_auth(struct auth_device *auth_dev);
int auth_device_get_batt_id(struct auth_device *auth_dev, u8 * id);
int auth_device_get_batt_sn(struct auth_device *auth_dev, u8 *soh_sn);
int auth_device_get_cycle_count(struct auth_device *auth_dev);
int auth_device_set_cycle_count(struct auth_device *auth_dev, int count, int get_count);
int auth_device_get_first_use_date(struct auth_device *auth_dev);
int auth_device_set_first_use_date(struct auth_device *auth_dev, int first_use_date);
int auth_device_get_batt_discharge_level(struct auth_device *auth_dev);
int auth_device_set_batt_discharge_level(struct auth_device *auth_dev, int batt_discharge_level);
int auth_device_get_batt_bsoh(struct auth_device *auth_dev);
int auth_device_set_batt_bsoh(struct auth_device *auth_dev, int bsoh);
int auth_device_get_batt_bsoh_raw(struct auth_device *auth_dev);
int auth_device_set_batt_bsoh_raw(struct auth_device *auth_dev, int bsoh_raw);
int auth_device_get_batt_full_status_usage(struct auth_device *auth_dev);
int auth_device_set_batt_full_status_usage(struct auth_device *auth_dev, int batt_full_status_usage);
int auth_device_get_batt_asoc(struct auth_device *auth_dev);
int auth_device_set_batt_asoc(struct auth_device *auth_dev, int asoc);
int auth_device_get_fai_expired(struct auth_device *auth_dev);
int auth_device_set_fai_expired(struct auth_device *auth_dev, int fai_expired);
int auth_device_get_sync_buf_mem_sts(struct auth_device *auth_dev);
int auth_device_set_sync_buf_mem_sts(struct auth_device *auth_dev, int sync_buf_mem_sts);
int auth_device_get_sync_buf_mem(struct auth_device *auth_dev);
int auth_device_set_sync_buf_mem(struct auth_device *auth_dev, int sync_buf_mem);

struct auth_device *auth_device_register(const char *name,
					 struct device *parent,
					 void *devdata,
					 const struct auth_ops *ops);

void auth_device_unregister(struct auth_device *auth_dev);
struct auth_device *get_batt_auth_by_name(const char *name);
#endif				/* __BATT_AUTH_CLASS__ */