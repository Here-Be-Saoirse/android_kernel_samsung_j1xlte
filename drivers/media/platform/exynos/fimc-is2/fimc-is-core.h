/*
 * Samsung Exynos5 SoC series FIMC-IS driver
 *
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FIMC_IS_CORE_H
#define FIMC_IS_CORE_H

#include <linux/version.h>
#include <linux/sched.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
#include <linux/sched/rt.h>
#endif
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/pm_runtime.h>
#include <mach/exynos-fimc-is2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>
#if defined(CONFIG_VIDEOBUF2_CMA_PHYS)
#include <media/videobuf2-cma-phys.h>
#elif defined(CONFIG_VIDEOBUF2_ION)
#include <media/videobuf2-ion.h>
#endif
#ifdef CONFIG_COMPANION_USE
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "fimc-is-companion.h"
#ifdef CONFIG_SOC_EXYNOS5422
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#endif
#endif

#include "fimc-is-param.h"
#include "fimc-is-interface.h"
#include "fimc-is-framemgr.h"
#include "fimc-is-resourcemgr.h"
#include "fimc-is-device-sensor.h"
#include "fimc-is-device-ischain.h"
#ifndef ENABLE_IS_CORE
#include "hardware/fimc-is-hw-control.h"
#include "interface/fimc-is-interface-ischain.h"
#endif
#include "fimc-is-device-companion.h"
#include "fimc-is-video.h"
#include "fimc-is-mem.h"
#include "vendor/fimc-is-spi.h"

#define FIMC_IS_DRV_NAME			"exynos-fimc-is"
#define FIMC_IS_COMMAND_TIMEOUT			(3*HZ)
#define FIMC_IS_STARTUP_TIMEOUT			(3*HZ)
#define FIMC_IS_COMPANION_TIMEOUT		(1*HZ)
#define FIMC_IS_SHUTDOWN_TIMEOUT		(10*HZ)
#define FIMC_IS_FLITE_STOP_TIMEOUT		(3*HZ)

#define FIMC_IS_SENSOR_MAX_ENTITIES		(1)
#define FIMC_IS_SENSOR_PAD_SOURCE_FRONT		(0)
#define FIMC_IS_SENSOR_PADS_NUM			(1)

#define FIMC_IS_FRONT_MAX_ENTITIES		(1)
#define FIMC_IS_FRONT_PAD_SINK			(0)
#define FIMC_IS_FRONT_PAD_SOURCE_BACK		(1)
#define FIMC_IS_FRONT_PAD_SOURCE_BAYER		(2)
#define FIMC_IS_FRONT_PAD_SOURCE_SCALERC	(3)
#define FIMC_IS_FRONT_PADS_NUM			(4)

#define FIMC_IS_BACK_MAX_ENTITIES		(1)
#define FIMC_IS_BACK_PAD_SINK			(0)
#define FIMC_IS_BACK_PAD_SOURCE_3DNR		(1)
#define FIMC_IS_BACK_PAD_SOURCE_SCALERP		(2)
#define FIMC_IS_BACK_PADS_NUM			(3)

#define FIMC_IS_MAX_SENSOR_NAME_LEN		(16)

#ifdef ENABLE_IS_CORE
#ifdef SUPPORTED_A5_MEMORY_SIZE_UP
#define FIMC_IS_A5_MEM_SIZE		(0x02000000)
#define FIMC_IS_BACKUP_SIZE		(0x02000000)
#else
#define FIMC_IS_A5_MEM_SIZE		(0x01A00000)
#define FIMC_IS_BACKUP_SIZE		(0x01A00000)
#endif
#define FIMC_IS_REGION_SIZE		(0x00005000)
#define FIMC_IS_SETFILE_SIZE		(0x00140000)
#define FIMC_IS_SHARED_REGION_ADDR	(0x019C0000)

#define FIMC_IS_REAR_CALDATA_OFFSET	(FIMC_IS_CAL_START_ADDR)
#define FIMC_IS_FRONT_CALDATA_OFFSET	(FIMC_IS_CAL_START_ADDR)

/* use sysfs for actuator */
#define INIT_MAX_SETTING					5
#else /* #ifdef ENABLE_IS_CORE */

#define FIMC_IS_GUARD_SIZE		(0x00001000)

#define FIMC_IS_SETFILE_OFFSET		(0x00001000)
#define FIMC_IS_SETFILE_SIZE		(0x00100000)

#define FIMC_IS_REAR_CALDATA_OFFSET	(FIMC_IS_SETFILE_OFFSET + \
					FIMC_IS_SETFILE_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_REAR_CALDATA_SIZE	(0x00010000)

#define FIMC_IS_FRONT_CALDATA_OFFSET	(FIMC_IS_REAR_CALDATA_OFFSET + \
					FIMC_IS_REAR_CALDATA_SIZE)
#define FIMC_IS_FRONT_CALDATA_SIZE	(0x00010000)

#define FIMC_IS_DEBUG_REGION_OFFSET	(FIMC_IS_FRONT_CALDATA_OFFSET + \
					FIMC_IS_FRONT_CALDATA_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_DEBUG_REGION_SIZE	(0x0007D000)

#define FIMC_IS_SHARED_REGION_OFFSET	(FIMC_IS_DEBUG_REGION_OFFSET + \
					FIMC_IS_DEBUG_REGION_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_SHARED_REGION_SIZE	(0x00010000)

#define FIMC_IS_DATA_REGION_OFFSET	(FIMC_IS_SHARED_REGION_OFFSET + \
					FIMC_IS_SHARED_REGION_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_DATA_REGION_SIZE	(0x00010000)

#define FIMC_IS_PARAM_REGION_OFFSET	(FIMC_IS_DATA_REGION_OFFSET + \
					FIMC_IS_DATA_REGION_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_PARAM_REGION_SIZE	(0x00005000)

/* for compatibility */
#define FIMC_IS_A5_MEM_SIZE		(FIMC_IS_PARAM_REGION_OFFSET + \
					(4*FIMC_IS_PARAM_REGION_SIZE))
#define FIMC_IS_REGION_SIZE		(FIMC_IS_PARAM_REGION_SIZE)

/* for internal DMA buffers */
#define FIMC_IS_THUMBNAIL_SDMA_OFFSET	(FIMC_IS_A5_MEM_SIZE + FIMC_IS_GUARD_SIZE)
#define FIMC_IS_THUMBNAIL_SDMA_SIZE	(0x00400000)

#define FIMC_IS_LHFD_MAP_OFFSET		(FIMC_IS_THUMBNAIL_SDMA_OFFSET + \
					FIMC_IS_THUMBNAIL_SDMA_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_LHFD_MAP_SIZE		(0x009F0000)

#define FIMC_IS_RESERVE_OFFSET		(FIMC_IS_LHFD_MAP_OFFSET + \
					FIMC_IS_LHFD_MAP_SIZE + \
					FIMC_IS_GUARD_SIZE)
#define FIMC_IS_RESERVE_SIZE		(0x00220000)

/* use sysfs for actuator */
#define INIT_MAX_SETTING					5


#ifndef CONFIG_USE_VENDER_FEATURE
/* EEPROM offset */
#define EEPROM_HEADER_BASE                    (0)
#define EEPROM_OEM_BASE                       (0x100)
#define EEPROM_AWB_BASE                       (0x200)
#define EEPROM_SHADING_BASE                   (0x300)
#define EEPROM_PDAF_BASE                      (0x0)

/* FROM offset */
#define FROM_HEADER_BASE                    (0)
#define FROM_OEM_BASE                       (0x1000)
#define FROM_AWB_BASE                       (0x2000)
#define FROM_SHADING_BASE                   (0x3000)
#define FROM_PDAF_BASE                      (0x5000)
#endif /* CONFIG_USE_VENDER_FEATURE */
#endif

#define FIMC_IS_FW_BASE_MASK		((1 << 26) - 1)

#define FW_SHARED_OFFSET		FIMC_IS_SHARED_REGION_ADDR

#define MAX_ODC_INTERNAL_BUF_WIDTH	(2560)  /* 4808 in HW */
#define MAX_ODC_INTERNAL_BUF_HEIGHT	(1920)  /* 3356 in HW */
#define SIZE_ODC_INTERNAL_BUF \
	(MAX_ODC_INTERNAL_BUF_WIDTH * MAX_ODC_INTERNAL_BUF_HEIGHT * 3)

#define MAX_DIS_INTERNAL_BUF_WIDTH	(2400)
#define MAX_DIS_INTERNAL_BUF_HEIGHT	(1360)
#define SIZE_DIS_INTERNAL_BUF \
	(MAX_DIS_INTERNAL_BUF_WIDTH * MAX_DIS_INTERNAL_BUF_HEIGHT * 2)

#define MAX_3DNR_INTERNAL_BUF_WIDTH	(3840)
#define MAX_3DNR_INTERNAL_BUF_HEIGHT	(2160)
#define SIZE_DNR_INTERNAL_BUF \
	(MAX_3DNR_INTERNAL_BUF_WIDTH * MAX_3DNR_INTERNAL_BUF_HEIGHT * 2)

#define MAX_FD_INTERNEL_BUF_WIDTH	(640)
#define MAX_FD_INTERNEL_BUF_HEIGHT	(480)
#define MAX_FD_INTERNEL_BUF_SIZE \
	(MAX_FD_INTERNEL_BUF_WIDTH * MAX_FD_INTERNEL_BUF_HEIGHT)
#define SIZE_FD_INTERNEL_BUF \
	((MAX_FD_INTERNEL_BUF_SIZE * 45 / 4) + (4096 + 1024))
/*
 * FD one buffer size: 3.4 MB
 * FD_ map_data_1: MAX_FD_INTERNEL_BUF_SIZE * 3 / 2) byte
 * FD_ map_data_2: MAX_FD_INTERNEL_BUF_SIZE * 4) byte
 * FD_ map_data_3: MAX_FD_INTERNEL_BUF_SIZE * 4) byte
 * FD_ map_data_4: MAX_FD_INTERNEL_BUF_SIZE / 4) byte
 * FD_ map_data_5: MAX_FD_INTERNEL_BUF_SIZE) byte
 * FD_ map_data_6: 1024 byte
 * FD_ map_data_7: 256 byte
 */
#define NUM_FD_INTERNAL_BUF		(3)

#define NUM_ODC_INTERNAL_BUF		(2)
#define NUM_DIS_INTERNAL_BUF		(1)
#define NUM_DNR_INTERNAL_BUF		(2)

#define GATE_IP_ISP			(0)
#define GATE_IP_DRC			(1)
#define GATE_IP_FD			(2)
#define GATE_IP_SCC			(3)
#define GATE_IP_SCP			(4)
#define GATE_IP_ODC			(0)
#define GATE_IP_DIS			(1)
#define GATE_IP_DNR			(2)
#if defined(CONFIG_SOC_EXYNOS3475)
#define DVFS_L0				(600000)
#define DVFS_L1				(500000)
#define DVFS_L1_1			(480000)
#define DVFS_L1_2			(460000)
#define DVFS_L1_3			(440000)

#define DVFS_MIF_L0			(800000)
#define DVFS_MIF_L1			(733000)
#define DVFS_MIF_L2			(667000)
#define DVFS_MIF_L3			(533000)
#define DVFS_MIF_L4			(400000)
#define DVFS_MIF_L5			(266000)

#define I2C_L0				(83000000)
#define I2C_L1				(36000000)
#define I2C_L1_1			(54000000)
#define I2C_L2				(21600000)
#define DVFS_SKIP_FRAME_NUM		(5)
#endif

#define I2C_RETRY_COUNT         5

#ifndef ENABLE_IS_CORE
/* use sysfs for actuator */
#define INIT_MAX_SETTING					5
#endif

enum fimc_is_debug_device {
	FIMC_IS_DEBUG_MAIN = 0,
	FIMC_IS_DEBUG_EC,
	FIMC_IS_DEBUG_SENSOR,
	FIMC_IS_DEBUG_ISP,
	FIMC_IS_DEBUG_DRC,
	FIMC_IS_DEBUG_FD,
	FIMC_IS_DEBUG_SDK,
	FIMC_IS_DEBUG_SCALERC,
	FIMC_IS_DEBUG_ODC,
	FIMC_IS_DEBUG_DIS,
	FIMC_IS_DEBUG_TDNR,
	FIMC_IS_DEBUG_SCALERP
};

enum fimc_is_debug_target {
	FIMC_IS_DEBUG_UART = 0,
	FIMC_IS_DEBUG_MEMORY,
	FIMC_IS_DEBUG_DCC3
};

enum fimc_is_front_input_entity {
	FIMC_IS_FRONT_INPUT_NONE = 0,
	FIMC_IS_FRONT_INPUT_SENSOR,
};

enum fimc_is_front_output_entity {
	FIMC_IS_FRONT_OUTPUT_NONE = 0,
	FIMC_IS_FRONT_OUTPUT_BACK,
	FIMC_IS_FRONT_OUTPUT_BAYER,
	FIMC_IS_FRONT_OUTPUT_SCALERC,
};

enum fimc_is_back_input_entity {
	FIMC_IS_BACK_INPUT_NONE = 0,
	FIMC_IS_BACK_INPUT_FRONT,
};

enum fimc_is_back_output_entity {
	FIMC_IS_BACK_OUTPUT_NONE = 0,
	FIMC_IS_BACK_OUTPUT_3DNR,
	FIMC_IS_BACK_OUTPUT_SCALERP,
};

enum fimc_is_front_state {
	FIMC_IS_FRONT_ST_POWERED = 0,
	FIMC_IS_FRONT_ST_STREAMING,
	FIMC_IS_FRONT_ST_SUSPENDED,
};

enum fimc_is_clck_gate_mode {
	CLOCK_GATE_MODE_HOST = 0,
	CLOCK_GATE_MODE_FW,
};

struct fimc_is_sysfs_debug {
	unsigned int en_dvfs;
	unsigned int en_clk_gate;
	unsigned int clk_gate_mode;
};

#ifndef ENABLE_IS_CORE
struct fimc_is_sysfs_actuator {
	unsigned int init_step;
	int init_positions[INIT_MAX_SETTING];
	int init_delays[INIT_MAX_SETTING];
};
#endif

struct fimc_is_core {
	struct platform_device			*pdev;
	struct resource				*regs_res;
	void __iomem				*regs;
	int					irq;
	u32					current_position;
	atomic_t				rsccount;
	unsigned long				state;

	/* depended on isp */
	struct exynos_platform_fimc_is		*pdata;

	struct fimc_is_resourcemgr		resourcemgr;
	struct fimc_is_groupmgr			groupmgr;
	struct fimc_is_interface		interface;

	struct fimc_is_device_sensor		sensor[FIMC_IS_SENSOR_COUNT];
	struct fimc_is_device_ischain		ischain[FIMC_IS_STREAM_COUNT];
	struct fimc_is_device_companion		companion;
#ifndef ENABLE_IS_CORE
	struct fimc_is_hardware			hardware;
	struct fimc_is_interface_ischain	interface_ischain;
#endif

	struct v4l2_device			v4l2_dev;

	struct fimc_is_video			video_30s;
	struct fimc_is_video			video_30c;
	struct fimc_is_video			video_30p;
	struct fimc_is_video			video_31s;
	struct fimc_is_video			video_31c;
	struct fimc_is_video			video_31p;
	struct fimc_is_video			video_i0s;
	struct fimc_is_video			video_i0c;
	struct fimc_is_video			video_i0p;
	struct fimc_is_video			video_i1s;
	struct fimc_is_video			video_i1c;
	struct fimc_is_video			video_i1p;
	struct fimc_is_video			video_scc;
	struct fimc_is_video			video_scp;
	struct fimc_is_video			video_vdc;
	struct fimc_is_video			video_vdo;

	/* spi */
	struct fimc_is_spi			spi0;
	struct fimc_is_spi			spi1;

//#if defined(CONFIG_COMPANION_USE)
	struct i2c_client			*client0;
//#endif
#if defined(CONFIG_OIS_USE)
	struct i2c_client			*client1;
#endif
#ifdef CONFIG_AF_HOST_CONTROL
	struct i2c_client			*client2;
#endif
#ifdef CONFIG_CAMERA_USE_SOC_SENSOR
	struct i2c_client			*client_soc;
#endif
	struct i2c_client			*eeprom_client0;
	struct i2c_client			*eeprom_client1;

#if defined(CONFIG_USE_DIRECT_IS_CONTROL) && defined(CONFIG_CAMERA_OTPROM_SUPPORT_FRONT)
	struct i2c_client			*front_cis_client;
#endif
#if defined(CONFIG_USE_DIRECT_IS_CONTROL) && defined(CONFIG_CAMERA_OTPROM_SUPPORT_REAR)
	struct i2c_client			*rear_cis_client;
#endif

#ifdef CONFIG_COMPANION_DCDC_USE
	struct dcdc_power			companion_dcdc;
#endif
	struct mutex			spi_lock;
#ifdef CONFIG_OIS_USE
	bool			ois_ver_read;
#endif /* CONFIG_OIS_USE */
	bool			use_ois_hsi2c;
	bool			use_module_check;
#ifdef USE_ION_ALLOC
	struct ion_client    *fimc_ion_client;
#endif
	bool			running_rear_camera;
	bool			running_front_camera;
};

#if defined(CONFIG_VIDEOBUF2_CMA_PHYS)
extern const struct fimc_is_vb2 fimc_is_vb2_cma;
#elif defined(CONFIG_VIDEOBUF2_ION)
extern const struct fimc_is_vb2 fimc_is_vb2_ion;
#endif

void fimc_is_mem_suspend(void *alloc_ctxes);
void fimc_is_mem_resume(void *alloc_ctxes);
void fimc_is_mem_cache_clean(const void *start_addr, unsigned long size);
void fimc_is_mem_cache_inv(const void *start_addr, unsigned long size);
int fimc_is_init_set(struct fimc_is_core *dev , u32 val);
int fimc_is_load_fw(struct fimc_is_core *dev);
int fimc_is_load_setfile(struct fimc_is_core *dev);
int fimc_is_otf_close(struct fimc_is_device_ischain *ischain);

#define CALL_POPS(s, op, args...) (((s) && (s)->pdata && (s)->pdata->op) ? ((s)->pdata->op((s)->pdev)) : -EPERM)

#endif /* FIMC_IS_CORE_H_ */
