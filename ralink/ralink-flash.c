/*
 *  This Code was picked from project: https://code.google.com/p/rt-n56u/ 
 *  to fix the following three symbols missing in ralink's wifi driver rt2860v2 
 *  
 *	EXPORT_SYMBOL(procRegDir);
 *	EXPORT_SYMBOL(ra_mtd_write_nm);
 *	EXPORT_SYMBOL(ra_mtd_read_nm);
 ***************************************************************************
 *	www.diveinedu.cn
 *  Cheetah 大茶园丁 147957232@qq.com
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/backing-dev.h>
#include <linux/compat.h>
#include <linux/mount.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/concat.h>
#include <linux/mtd/partitions.h>
#include "../mtdcore.h"
#include <asm/io.h>
#include <asm/addrspace.h>
/*
#include <asm/rt2880/rt_mmap.h>
*/
#include "ralink-flash.h"

#define WINDOW_ADDR		CPHYSADDR(CONFIG_RT2880_MTD_PHYSMAP_START)
#define WINDOW_SIZE		CONFIG_RT2880_MTD_PHYSMAP_LEN
#define NUM_FLASH_BANKS		1

#define BUSWIDTH		CONFIG_RT2880_MTD_PHYSMAP_BUSWIDTH



static int __init rt2880_mtd_init(void)
{

	return 0;
}

static void __exit rt2880_mtd_exit(void)
{
}

/*
 * Flash API: ra_mtd_read, ra_mtd_write
 * Arguments:
 *   - num: specific the mtd number
 *   - to/from: the offset to read from or written to
 *   - len: length
 *   - buf: data to be read/written
 * Returns:
 *   - return -errno if failed
 *   - return the number of bytes read/written if successed
 */
int ra_mtd_write_nm(char *name, loff_t to, size_t len, const u_char *buf)
{
	int ret = -1;
	size_t rdlen, wrlen;
	struct mtd_info *mtd;
	struct erase_info ei;
	u_char *bak = NULL;

	mtd = get_mtd_device_nm(name);

	if (IS_ERR(mtd)) {
		ret = (int)mtd;
		goto out;
	}

	if (len > mtd->erasesize) {
		put_mtd_device(mtd);
		ret = -E2BIG;
		goto out;
	}

	bak = kzalloc(mtd->erasesize, GFP_KERNEL);
	if (bak == NULL) {
		put_mtd_device(mtd);
		ret = -ENOMEM;
		goto out;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
	ret = mtd_read(mtd, 0, mtd->erasesize, &rdlen, bak);
#else
	ret = mtd->read(mtd, 0, mtd->erasesize, &rdlen, bak);
#endif
	if (ret) {
		goto free_out;
	}

	if (rdlen != mtd->erasesize)
		printk("warning: ra_mtd_write_nm: rdlen is not equal to erasesize\n");

	memcpy(bak + to, buf, len);

	ei.mtd = mtd;
	ei.callback = NULL;
	ei.addr = 0;
	ei.len = mtd->erasesize;
	ei.priv = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
	ret = mtd_erase(mtd, &ei);
#else
	ret = mtd->erase(mtd, &ei);
#endif
	if (ret != 0)
		goto free_out;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
	ret = mtd_write(mtd, 0, mtd->erasesize, &wrlen, bak);
#else
	ret = mtd->write(mtd, 0, mtd->erasesize, &wrlen, bak);
#endif

	udelay(10); /* add delay after write */

free_out:
	if (mtd)
		put_mtd_device(mtd);

	if (bak)
		kfree(bak);
out:
	return ret;
}

int ra_mtd_read_nm(char *name, loff_t from, size_t len, u_char *buf)
{
	int ret;
	size_t rdlen = 0;
	struct mtd_info *mtd;

	mtd = get_mtd_device_nm(name);
	if (IS_ERR(mtd))
		return (int)mtd;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
	ret = mtd_read(mtd, from, len, &rdlen, buf);
#else
	ret = mtd->read(mtd, from, len, &rdlen, buf);
#endif
	if (rdlen != len)
		printk("warning: ra_mtd_read_nm: rdlen is not equal to len\n");

	put_mtd_device(mtd);
	return ret;
}

module_init(rt2880_mtd_init);
module_exit(rt2880_mtd_exit);
struct proc_dir_entry *procRegDir;
EXPORT_SYMBOL(procRegDir);
EXPORT_SYMBOL(ra_mtd_write_nm);
EXPORT_SYMBOL(ra_mtd_read_nm);

MODULE_AUTHOR("Steven Liu <steven_liu@ralinktech.com.tw>");
MODULE_DESCRIPTION("Ralink APSoC Flash Map");
MODULE_LICENSE("GPL");
