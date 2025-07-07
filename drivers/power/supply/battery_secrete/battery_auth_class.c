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
#include "battery_auth_class.h"

static struct class *auth_class;
static uint16_t gpio;
//static struct gpio_desc *gpiod;
//struct regulator *vreg;

static ssize_t name_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	struct auth_device *auth = to_auth_device(dev);

	return snprintf(buf, 20, "%s\n",
			auth->name ? auth->name : "anonymous");
}

static DEVICE_ATTR_RO(name);

static struct attribute *auth_attrs[] = {
	&dev_attr_name.attr,
	NULL,
};

static const struct attribute_group auth_group = {
	.attrs = auth_attrs,
};

static const struct attribute_group *auth_groups[] = {
	&auth_group,
	NULL,
};

int auth_device_start_auth(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
	    auth_dev->ops->auth_battery != NULL)
		return auth_dev->ops->auth_battery(auth_dev);

	return -EOPNOTSUPP;
}

EXPORT_SYMBOL(auth_device_start_auth);

int auth_device_get_batt_id(struct auth_device *auth_dev, u8 * id)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
	    auth_dev->ops->get_battery_id != NULL)
		return auth_dev->ops->get_battery_id(auth_dev, id);

	return -EOPNOTSUPP;
}

EXPORT_SYMBOL(auth_device_get_batt_id);

int auth_device_get_batt_sn(struct auth_device *auth_dev, u8 *soh_sn)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
	    auth_dev->ops->get_batt_sn != NULL) {
		pr_info("%s enter\n", __func__);
		return auth_dev->ops->get_batt_sn(auth_dev, soh_sn);
	}

	return -EOPNOTSUPP;
}

EXPORT_SYMBOL(auth_device_get_batt_sn);

int auth_device_get_cycle_count(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_cycle_count != NULL)
		return auth_dev->ops->get_cycle_count(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_cycle_count);

int auth_device_set_cycle_count(struct auth_device *auth_dev, int count, int get_count)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_cycle_count != NULL)
		return auth_dev->ops->set_cycle_count(auth_dev, count, get_count);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_cycle_count);

int auth_device_get_first_use_date(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_first_use_date != NULL)
		return auth_dev->ops->get_first_use_date(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_first_use_date);

int auth_device_set_first_use_date(struct auth_device *auth_dev, int first_use_date)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_first_use_date != NULL)
		return auth_dev->ops->set_first_use_date(auth_dev, first_use_date);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_first_use_date);

int auth_device_get_batt_bsoh(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_batt_bsoh != NULL)
		return auth_dev->ops->get_batt_bsoh(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_batt_bsoh);

int auth_device_set_batt_bsoh(struct auth_device *auth_dev, int bsoh)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_batt_bsoh != NULL)
		return auth_dev->ops->set_batt_bsoh(auth_dev, bsoh);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_batt_bsoh);

int auth_device_get_batt_bsoh_raw(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_batt_bsoh_raw != NULL)
		return auth_dev->ops->get_batt_bsoh_raw(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_batt_bsoh_raw);

int auth_device_set_batt_bsoh_raw(struct auth_device *auth_dev, int bsoh_raw)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_batt_bsoh_raw != NULL)
		return auth_dev->ops->set_batt_bsoh_raw(auth_dev, bsoh_raw);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_batt_bsoh_raw);

int auth_device_get_batt_asoc(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_batt_asoc != NULL)
		return auth_dev->ops->get_batt_asoc(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_batt_asoc);

int auth_device_set_batt_asoc(struct auth_device *auth_dev, int bsoh)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_batt_asoc != NULL)
		return auth_dev->ops->set_batt_asoc(auth_dev, bsoh);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_batt_asoc);

int auth_device_get_fai_expired(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_fai_expired != NULL)
		return auth_dev->ops->get_fai_expired(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_fai_expired);

int auth_device_set_fai_expired(struct auth_device *auth_dev, int fai_expired)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_fai_expired != NULL)
		return auth_dev->ops->set_fai_expired(auth_dev, fai_expired);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_fai_expired);

int auth_device_get_sync_buf_mem_sts(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_sync_buf_mem_sts != NULL)
		return auth_dev->ops->get_sync_buf_mem_sts(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_sync_buf_mem_sts);

int auth_device_set_sync_buf_mem_sts(struct auth_device *auth_dev, int sync_buf_mem_sts)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_sync_buf_mem_sts != NULL)
		return auth_dev->ops->set_sync_buf_mem_sts(auth_dev, sync_buf_mem_sts);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_sync_buf_mem_sts);

int auth_device_get_sync_buf_mem(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_sync_buf_mem != NULL)
		return auth_dev->ops->get_sync_buf_mem(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_sync_buf_mem);

int auth_device_set_sync_buf_mem(struct auth_device *auth_dev, int sync_buf_mem)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_sync_buf_mem != NULL)
		return auth_dev->ops->set_sync_buf_mem(auth_dev, sync_buf_mem);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_sync_buf_mem);

int auth_device_get_batt_full_status_usage(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_batt_full_status_usage != NULL)
		return auth_dev->ops->get_batt_full_status_usage(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_batt_full_status_usage);

int auth_device_set_batt_full_status_usage(struct auth_device *auth_dev, int batt_full_status_usage)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_batt_full_status_usage != NULL)
		return auth_dev->ops->set_batt_full_status_usage(auth_dev, batt_full_status_usage);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_batt_full_status_usage);

int auth_device_get_batt_discharge_level(struct auth_device *auth_dev)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->get_batt_discharge_level != NULL)
		return auth_dev->ops->get_batt_discharge_level(auth_dev);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_get_batt_discharge_level);

int auth_device_set_batt_discharge_level(struct auth_device *auth_dev, int batt_discharge_level)
{
	if (auth_dev != NULL && auth_dev->ops != NULL &&
		auth_dev->ops->set_batt_discharge_level != NULL)
		return auth_dev->ops->set_batt_discharge_level(auth_dev, batt_discharge_level);
	return -EOPNOTSUPP;
}
EXPORT_SYMBOL(auth_device_set_batt_discharge_level);

static void auth_device_release(struct device *dev)
{
	struct auth_device *auth_dev = to_auth_device(dev);

	kfree(auth_dev);
}

struct auth_device *auth_device_register(const char *name,
					 struct device *parent,
					 void *devdata,
					 const struct auth_ops *ops)
{
	struct auth_device *auth_dev;
	int rc;

	pr_info("%s: name = %s\n", __func__, name);
	auth_dev = kzalloc(sizeof(*auth_dev), GFP_KERNEL);
	if (!auth_dev)
		return ERR_PTR(-ENOMEM);

	raw_spin_lock_init(&auth_dev->io_lock);

	auth_dev->dev.class = auth_class;
	auth_dev->dev.parent = parent;
	auth_dev->dev.release = auth_device_release;
	auth_dev->gpio = gpio;
	dev_set_name(&auth_dev->dev, "main_supplier");
	dev_set_drvdata(&auth_dev->dev, devdata);

	rc = device_register(&auth_dev->dev);
	if (rc) {
		kfree(auth_dev);
		return ERR_PTR(rc);
	}

	auth_dev->ops = ops;
	pr_info("%s: end!!!\n", __func__);

	return auth_dev;
}

EXPORT_SYMBOL(auth_device_register);

void auth_device_unregister(struct auth_device *auth_dev)
{
	if (!auth_dev)
		return;

	auth_dev->ops = NULL;

	device_unregister(&auth_dev->dev);
}

EXPORT_SYMBOL(auth_device_unregister);

static int auth_match_device_by_name(struct device *dev, const void *data)
{
	const char *name = data;

	return strcmp(dev_name(dev), name) == 0;
}

struct auth_device *get_batt_auth_by_name(const char *name)
{
	struct device *dev;

	if (!name)
		return (struct auth_device *) NULL;

	dev = class_find_device(auth_class, NULL, name,
				auth_match_device_by_name);

	return dev ? to_auth_device(dev) : NULL;
}

EXPORT_SYMBOL(get_batt_auth_by_name);

static int onewire_gpio_probe(struct platform_device *pdev)
{
	//int status = 0;

	auth_class = class_create(THIS_MODULE, "battery_auth");
	if (IS_ERR(auth_class)) {
		pr_err("Unable to create auth class\n");
		return -ENODEV;
	}

	auth_class->dev_groups = auth_groups;

	/*gpiod = devm_gpiod_get_index(&(pdev->dev), NULL, 0, gflags);
	if (IS_ERR(gpiod)) {
		pr_err("%s gpio_request (pin) failed\n", __func__);
		return PTR_ERR(gpiod);
	}

	gpiod_direction_output(gpiod, 1);
	vreg = regulator_get(&(pdev->dev), DTS_VOlT_REGULATER);
	if (IS_ERR(vreg)) {
		pr_err("%s get vreg fail\n", __func__);
		return -EPERM;
	}

	status = regulator_set_voltage(vreg, 3300000, 3300000);
	status = regulator_enable(vreg);
	status = regulator_get_voltage(vreg);
	pr_err("power on regulator_value %d!!\n", status);*/
	pr_err("%s succ\n", __func__);
	return 0;
}

static const struct of_device_id onewire_gpio_dt_match[] = {
	{.compatible = "samsung,onewire_gpio"},
	{},
};

static struct platform_driver onewire_gpio_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "onewire_gpio",
		   .of_match_table = onewire_gpio_dt_match,
		   },
	.probe = onewire_gpio_probe,
};

static int __init batt_auth_class_init(void)
{
	return platform_driver_register(&onewire_gpio_driver);
}

static void __exit batt_auth_class_exit(void)
{
	platform_driver_unregister(&onewire_gpio_driver);
}

module_init(batt_auth_class_init);
module_exit(batt_auth_class_exit);

MODULE_LICENSE("GPL");
