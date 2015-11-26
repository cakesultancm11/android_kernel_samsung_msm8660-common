/* drivers/leds/ledtrig-sleep.c
 *
 * Copyright (C) 2007 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifdef CONFIG_LCD_NOTIFY
#include <linux/lcd_notify.h>
#endif
#include <linux/leds.h>
#include <linux/suspend.h>

static int ledtrig_sleep_pm_callback(struct notifier_block *nfb,
					unsigned long action,
					void *ignored);

#ifdef CONFIG_LCD_NOTIFY
static struct notifier_block ledtrig_lcd_notif;
#endif

DEFINE_LED_TRIGGER(ledtrig_sleep)
static struct notifier_block ledtrig_sleep_pm_notifier = {
	.notifier_call = ledtrig_sleep_pm_callback,
	.priority = 0,
};

#ifdef CONFIG_LCD_NOTIFY
static int ledtrig_lcd_notifier_callback(struct notifier_block *this,
				unsigned long event, void *data) {
	pr_debug("%s: event = %lu\n", __func__, event);

	switch (event) {
	case LCD_EVENT_ON_START:
		led_trigger_event(ledtrig_sleep, LED_OFF);
		break;
	case LCD_EVENT_ON_END:
		break;
	case LCD_EVENT_OFF_START:
		break;
	case LCD_EVENT_OFF_END:
		led_trigger_event(ledtrig_sleep, LED_FULL);
		break;
	default:
		break;
	}

	return 0;
}
#endif

static int ledtrig_sleep_pm_callback(struct notifier_block *nfb,
					unsigned long action,
					void *ignored)
{
	switch (action) {
	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		led_trigger_event(ledtrig_sleep, LED_OFF);
		return NOTIFY_OK;
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
		led_trigger_event(ledtrig_sleep, LED_FULL);
		return NOTIFY_OK;
	}

	return NOTIFY_DONE;
}

static int __init ledtrig_sleep_init(void)
{
	int ret = 0;
	led_trigger_register_simple("sleep", &ledtrig_sleep);
	register_pm_notifier(&ledtrig_sleep_pm_notifier);
#ifdef CONFIG_LCD_NOTIFY
	ledtrig_lcd_notif.notifier_call = ledtrig_lcd_notifier_callback;
	if (lcd_register_client(&ledtrig_lcd_notif) != 0) {
		pr_err("%s: Failed to register lcd callback\n", __func__);
		lcd_unregister_client(&ledtrig_lcd_notif);
		ret = -EINVAL;
	}
#endif
	return ret;
}

static void __exit ledtrig_sleep_exit(void)
{
#ifdef CONFIG_LCD_NOTIFY
	lcd_unregister_client(&ledtrig_lcd_notif);
#endif
	unregister_pm_notifier(&ledtrig_sleep_pm_notifier);
	led_trigger_unregister_simple(ledtrig_sleep);
}

module_init(ledtrig_sleep_init);
module_exit(ledtrig_sleep_exit);

