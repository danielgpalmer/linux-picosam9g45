/*
 *  Board-specific setup code for the pico-SAM9G45
 *
 *  Based on board-sam9m10g45ek.c 
 *
 *  Copyright (C) 2011 Daniel Palmer. <me@0x0f.com>
 *  Copyright (C) 2009 Atmel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/fb.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <linux/clk.h>
#include <linux/atmel-mci.h>

#include <mach/hardware.h>
#include <video/atmel_lcdc.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/at91sam9_smc.h>
#include <mach/at91_shdwc.h>
#include <mach/system_rev.h>

#include "sam9_smc.h"
#include "generic.h"


static void __init picosam_init_early(void)
{
	/* Initialize processor: 12.000 MHz crystal */
	at91_initialize(12000000);

	/* DGBU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* USART0 not connected on the -EK board */
	/* USART1 on ttyS2. (Rx, Tx, RTS, CTS) */
	at91_register_uart(AT91SAM9G45_ID_US1, 2, ATMEL_UART_CTS | ATMEL_UART_RTS);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}

/*
 * USB HS Host port (common to OHCI & EHCI)
 */
static struct at91_usbh_data __initdata ek_usbh_hs_data = {
	.ports		= 2,
	.vbus_pin	= {AT91_PIN_PD1, AT91_PIN_PD3},
};


/*
 * USB HS Device port
 */
static struct usba_platform_data __initdata ek_usba_udc_data = {
	.vbus_pin	= AT91_PIN_PB19,
};

/*
 * SPI devices.
 */
static struct spi_board_info picosam9g45_spi_devices[] = {
        [0] = {/* SPI0 CS0 on right side connector J7*/
                .modalias= "spidev",
                .max_speed_hz= 15 * 1000 * 1000,
                .bus_num= 0,
                .chip_select= 0,
        },
        [1] = {/* SPI1 CS0 on left side connector J9*/
                .modalias= "spidev",
                .max_speed_hz= 15 * 1000 * 1000,
                .bus_num= 1,
                .chip_select= 0,
        },
};

/*
 * MCI (SD/MMC)
 */
static struct mci_platform_data __initdata mci0_data = {
	.slot[0] = {
		.bus_width	= 4,
		.detect_pin	= AT91_PIN_PD10,
	},
};

static struct mci_platform_data __initdata mci1_data = {
	.slot[0] = {
		.bus_width	= 4,
		.detect_pin	= AT91_PIN_PD11,
		.wp_pin		= AT91_PIN_PD29,
	},
};


/*
 * MACB Ethernet device
 */
static struct at91_eth_data __initdata ek_macb_data = {
	.phy_irq_pin	= AT91_PIN_PD5,
	.is_rmii	= 1,
};

/*
 * LCD Controller
 */
#if defined(CONFIG_FB_ATMEL) || defined(CONFIG_FB_ATMEL_MODULE)
#warning "lcd stuff"
static struct fb_videomode at91_tft_vga_modes[] = {
        {
                .name           = "HannStar",
                .refresh        = 60,
                .xres           = 480,          .yres           = 272,
                .pixclock       = KHZ2PICOS(9000),

                .left_margin    = 2,            .right_margin   = 2,
                .upper_margin   = 2,            .lower_margin   = 2,
                .hsync_len      = 41,           .vsync_len      = 10,

                .sync           = 0,
                .vmode          = FB_VMODE_NONINTERLACED,
        },
};

static struct fb_monspecs at91fb_default_monspecs = {
        .manufacturer   = "HNS",
        .monitor        = "HSD043I9W1",

        .modedb         = at91_tft_vga_modes,
        .modedb_len     = ARRAY_SIZE(at91_tft_vga_modes),
        .hfmin          = 15000,
        .hfmax          = 17640,
        .vfmin          = 57,
        .vfmax          = 67,
};


#define AT91SAM9G45_DEFAULT_LCDCON2 	(ATMEL_LCDC_MEMOR_LITTLE \
					| ATMEL_LCDC_DISTYPE_TFT \
					| ATMEL_LCDC_CLKMOD_ALWAYSACTIVE)

/* Driver datas */
static struct atmel_lcdfb_info __initdata ek_lcdc_data = {
	.lcdcon_is_backlight		= true,
	.default_bpp			= 16,
	.default_dmacon			= ATMEL_LCDC_DMAEN,
	.default_lcdcon2		= AT91SAM9G45_DEFAULT_LCDCON2,
	.default_monspecs		= &at91fb_default_monspecs,
	.guard_time			= 9,
	.lcd_wiring_mode		= ATMEL_LCDC_WIRING_RGB,
};

#else
static struct atmel_lcdfb_info __initdata ek_lcdc_data;
#endif


/*
 * Touchscreen
 */
static struct at91_tsadcc_data ek_tsadcc_data = {
	.adc_clock		= 300000,
	.pendet_debounce	= 0x0d,
	.ts_sample_hold_time	= 0x0a,
};


/*
 * GPIO Buttons
 */
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button picosam_buttons[] = {

	/* SOMEONE NEEDS TO FIX THESE IF THEY WANT TO USE THEM */

	{	.code		= BTN_LEFT,
		.gpio		= AT91_PIN_PB6,
		.active_low	= 1,
		.desc		= "left_click",
		.wakeup		= 1,
	},

	{	.code		= BTN_RIGHT,
		.gpio		= AT91_PIN_PB7,
		.active_low	= 1,
		.desc		= "right_click",
		.wakeup		= 1,
	},

	{
		.code		= KEY_LEFT,
		.gpio		= AT91_PIN_PB14,
		.active_low	= 1,
		.desc		= "Joystick Left",
	},

	{
		.code		= KEY_RIGHT,
		.gpio		= AT91_PIN_PB15,
		.active_low	= 1,
		.desc		= "Joystick Right",
	},

	{
		.code		= KEY_UP,
		.gpio		= AT91_PIN_PB16,
		.active_low	= 1,
		.desc		= "Joystick Up",
	},

	{
		.code		= KEY_DOWN,
		.gpio		= AT91_PIN_PB17,
		.active_low	= 1,
		.desc		= "Joystick Down",
	},

	{
		.code		= KEY_ENTER,
		.gpio		= AT91_PIN_PB18,
		.active_low	= 1,
		.desc		= "Joystick Press",
	},

};

static struct gpio_keys_platform_data ek_button_data = {
	.buttons	= picosam_buttons,
	.nbuttons	= ARRAY_SIZE(picosam_buttons),
};

static struct platform_device picosam_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &ek_button_data,
	}
};

static void __init picosam_add_device_buttons(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(picosam_buttons); i++) {
		at91_set_GPIO_periph(picosam_buttons[i].gpio, 1);
		at91_set_deglitch(picosam_buttons[i].gpio, 1);
	}

	platform_device_register(&picosam_button_device);
}
#else
static void __init picosam_add_device_buttons(void) {}
#endif


/*
 * AC97
 * reset_pin is not connected: NRST
 */
static struct ac97c_platform_data ek_ac97_data = {
};


/*
 * LEDs; The board has pads for a few leds, but only u2 is populated
 */
static struct gpio_led picosam_leds[] = {
#if !(defined(CONFIG_LEDS_ATMEL_PWM) || defined(CONFIG_LEDS_ATMEL_PWM_MODULE))
	#warning "PWM support is disabled!";
	{	/* marked u2 on the board */
		.name			= "u2",
		.gpio			= AT91_PIN_PD31,
		.active_low		= 1,
		.default_trigger	= "mmc0",
	}

#endif
};


/*
 * PWM Leds; The buzzer isn't really an LED, but this works
 */
static struct gpio_led picosam_pwm_led[] = {
#if defined(CONFIG_LEDS_ATMEL_PWM) || defined(CONFIG_LEDS_ATMEL_PWM_MODULE)

	{       /* marked u2 on the board*/
                .name                   = "u2",
                .gpio                   = 1,    /* is PWM channel number */
                .active_low             = 1,
                .default_trigger        = "mmc0",
        },

        {       /* picopc buzzer, I guess this has to be PIO driven */
                .name                   = "buzzer",
                .gpio                   = 2,    // is PWM channel number 
                .active_low             = 1,
                .default_trigger        = "none",
        }

#endif
};



static void __init ek_board_init(void)
{
	/* Serial */
	at91_add_device_serial();
	/* USB HS Host */
	at91_add_device_usbh_ohci(&ek_usbh_hs_data);
	at91_add_device_usbh_ehci(&ek_usbh_hs_data);
	/* USB HS Device */
	at91_add_device_usba(&ek_usba_udc_data);
	/* SPI */
        at91_add_device_spi(picosam9g45_spi_devices, ARRAY_SIZE(picosam9g45_spi_devices));
	/* MMC */
	at91_add_device_mci(0, &mci0_data);
	at91_add_device_mci(1, &mci1_data);
	/* Ethernet */
	at91_add_device_eth(&ek_macb_data);
	/* I2C */
	at91_add_device_i2c(0, NULL, 0);
	/* LCD Controller */
	at91_add_device_lcdc(&ek_lcdc_data);
	/* Touch Screen */
	at91_add_device_tsadcc(&ek_tsadcc_data);
	/* Push Buttons */
	picosam_add_device_buttons();
	/* AC97 */
	at91_add_device_ac97(&ek_ac97_data);
	/* LEDs */
	at91_gpio_leds(picosam_leds, ARRAY_SIZE(picosam_leds));
	at91_pwm_leds(picosam_pwm_led, ARRAY_SIZE(picosam_pwm_led));
}

MACHINE_START(PICOSAM9G45, "Mini-Box.com pico-SAM9G45")
	/* Maintainer: Daniel Palmer */
	.timer		= &at91sam926x_timer,
	.map_io		= at91_map_io,
	.init_early	= picosam_init_early,
	.init_irq	= at91_init_irq_default,
	.init_machine	= ek_board_init,
MACHINE_END
