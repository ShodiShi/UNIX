#include <linux/kernel.h>   /* [cite: 36] */
#include <linux/module.h>   /* [cite: 37] */
#include <linux/proc_fs.h>  /* [cite: 37] */
#include <linux/uaccess.h>  /* [cite: 38] */
#include <linux/version.h>  /* [cite: 39] */
#include <linux/timekeeping.h> 

#define PROC_NAME "tsulab" /* [cite: 40] */

static struct proc_dir_entry *our_proc_file = NULL; /* [cite: 41] */

// 12 созвездий (сектора по 30 градусов)
static const char *const constellations[] = {
    "Aries", "Taurus", "Gemini", "Cancer", 
    "Leo", "Virgo", "Libra", "Scorpio", 
    "Sagittarius", "Capricorn", "Aquarius", "Pisces"
};

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) { /* [cite: 45, 47] */
    char s[128];
    int len;
    
    // Точка отсчета: 20.08.2023 (Unix timestamp: 1692489600)
    // В этот момент Юпитер вошел в сектор Овна.
    unsigned long long start_time = 1692489600; 
    unsigned long long jupiter_period = 374412000; // ~11.86 лет в секундах
    unsigned long long now = ktime_get_real_seconds();
    
    if (now < start_time) return 0;

    unsigned long long diff = now - start_time;
    // Сколько полных кругов прошел Юпитер от нашей даты
    unsigned long long cycles = diff / jupiter_period;
    // Текущий сектор (от 0 до 11)
    int sector = ((diff % jupiter_period) / (jupiter_period / 12)) % 12;

    if (*offset > 0) return 0; 

    len = snprintf(s, sizeof(s), "Constellation: %s\nCycles forward: %llu\n", 
                   constellations[sector], cycles);
    
    if (copy_to_user(buffer, s, len)) return -EFAULT; /*  */

    *offset += len; 
    return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops proc_file_fops = { .proc_read = procfile_read };
#else
static const struct file_operations proc_file_fops = { .read = procfile_read };
#endif

static int __init tsulab_init(void) { /* [cite: 68] */
    our_proc_file = proc_create(PROC_NAME, 0644, NULL, &proc_file_fops); /* [cite: 70, 71] */
    pr_info("Welcome to the Tomsk State University\n"); /* [cite: 16] */
    return 0;
}

static void __exit tsulab_exit(void) { /* [cite: 74] */
    proc_remove(our_proc_file); /* [cite: 76] */
    pr_info("Tomsk State University forever!\n"); /* [cite: 20] */
}

module_init(tsulab_init); /* [cite: 79] */
module_exit(tsulab_exit); /* [cite: 80] */
MODULE_LICENSE("GPL");    /* [cite: 81] */