/*
 * Copyright (c) 2018, UNISOC Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed_retarget.h"
#include "ipi.h"
#include "uwp_log.h"
#include "UWP_5661.h"

struct __ipi_cb {
	uwp_ipi_callback_t cb;
	void *data;
};

struct ipi_uwp_data {
	struct __ipi_cb irq;
};

static struct ipi_uwp_data ipi_uwp_dev_data;

void uwp_ipi_set_callback(uwp_ipi_callback_t cb, void *arg)
{
	if (ipi_uwp_dev_data.irq.cb != NULL) {
		LOG_DBG("ipi irq callback has been registered.\n");
		return;
	}

	ipi_uwp_dev_data.irq.cb = cb;
	ipi_uwp_dev_data.irq.data = arg;
	
}

void uwp_ipi_unset_callback(void)
{
	ipi_uwp_dev_data.irq.cb = NULL;
	ipi_uwp_dev_data.irq.data = NULL;
}

void ipi_uwp_irq(void)
{
	NVIC_DisableIRQ(GNSS2BTWIFI_IPI_IRQn);
	uwp_ipi_clear_remote(IPI_CORE_BTWF, IPI_TYPE_IRQ0);
	if (ipi_uwp_dev_data.irq.cb)
		ipi_uwp_dev_data.irq.cb(ipi_uwp_dev_data.irq.data);

	NVIC_EnableIRQ(GNSS2BTWIFI_IPI_IRQn);
}

void ipi_uwp_init(void)
{
    NVIC_DisableIRQ(GNSS2BTWIFI_IPI_IRQn);

	uwp_sys_enable(BIT(APB_EB_IPI));
	uwp_sys_reset(BIT(APB_EB_IPI));

// TODO: isr priority
    NVIC_SetPriority(GNSS2BTWIFI_IPI_IRQn,0x1FUL);
    NVIC_SetVector(GNSS2BTWIFI_IPI_IRQn,(uint32_t)ipi_uwp_irq);
	NVIC_EnableIRQ(GNSS2BTWIFI_IPI_IRQn);

	LOG_DBG("ipi initialized");
}

