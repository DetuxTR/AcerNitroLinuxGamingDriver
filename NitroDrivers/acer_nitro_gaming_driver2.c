#include "acer_nitro_gaming_driver2.h"
#include "linux/fs.h"
#include "linux/init.h"
#include "linux/kdev_t.h"
#include "linux/kern_levels.h"
#include "linux/kstrtox.h"
#include "linux/printk.h"
#include "linux/wmi.h"




MODULE_LICENSE("GPL");

#define MINORMASK 0xff
#define MAX_FAN_SPEED 10000

static int cmajor = 0;
static struct class *cclass = NULL;
static dev_t cdev;
static struct device *dev1 = NULL, *dev2 = NULL;
static int cmajor= 0;

static struct class *cclass=NULL;
dev_t cdev;
static const struct file_operations cfops={
    .owner=THIS_MODULE,
    .write= cdev_user_write,
    .open= chdev_open,
    .release= chdev_release

};
struct chdev_data{
    struct cdev cdev;
};
static struct chdev_data cdev_data[2];
extern int chdev_uevent(const struct device *dev,struct kobj_uevent_env *env){
    add_uevent_var(env, "DEVMODE=%#o",0666);
    return 0;
}

static int cdev_create(const char *name, int major, int minor, struct class *cls) {
    struct device *dev;
    dev = device_create(cls, NULL, MKDEV(major, minor), NULL, name);
    if (IS_ERR(dev)) {
        printk(KERN_ERR "Cihaz oluşturulamadı %s", name);
        return PTR_ERR(dev);
    }
    return 0;
}
ssize_t cdev_user_write(struct file * file,const char __user * buff, size_t count, loff_t *offset){
    int cdev_minor = MINOR(file->f_path.dentry->d_inode->i_rdev);
    int ret, ispeed = 0;
    char *kbfr = NULL;
    
    printk(KERN_INFO "writing to : %d", cdev_minor);

    kbfr = kmalloc(count, GFP_KERNEL);
    if(!kbfr)
        return -ENOMEM;

    // copy_from_user return value check
    if(copy_from_user(kbfr, buff, count)) {
        kfree(kbfr);
        return -EFAULT;
    }

    int ix = strnlen(kbfr, count);
    if(ix > 0)
        kbfr[ix-1] = '\0';

    printk(KERN_INFO "%s", kbfr);

    switch(cdev_minor){
        case 0:
            printk(KERN_INFO "CPUFAN");
            ret = kstrtoint(kbfr, 10, &ispeed);
            if(ret) {
                kfree(kbfr);
                return ret;
            }
            fan_set_speed(ispeed, 1);
            break;
            
        case 1:
            printk(KERN_INFO "GPUFAN");
            ret = kstrtoint(kbfr, 10, &ispeed);
            if(ret) {
                kfree(kbfr);
                return ret;
            }
            fan_set_speed(ispeed, 4);
            break;
    }
    
    kfree(kbfr); // Memory leak fix
    return count;
}
extern int chdev_open(struct inode * inode,struct file * file){
    try_module_get(THIS_MODULE);
    return 0;
}

extern int chdev_release(struct inode * inode,struct file * file){
    module_put(THIS_MODULE);
    return 0;
}
//Wmi Driver Definition
static struct wmi_device *w_dev ;

struct driver_data_t{};

static const struct wmi_device_id w_dev_id[] = {{
    .guid_string = WMI_GAMING_GUID
},
};

static struct wmi_driver wdrv = {
    .driver = {.owner = THIS_MODULE, .name = DRV_NAME, .probe_type=PROBE_PREFER_ASYNCHRONOUS},
    .id_table = w_dev_id,
    .remove = wmi_remove,
    .probe = wmi_probe,

};
void wmi_remove(struct wmi_device *w_devv) { w_dev = NULL; }

extern int wmi_probe(struct wmi_device *wdevv, const void *notuseful) {
    struct driver_data_t *driver_data;
    if(!wmi_has_guid(WMI_GAMING_GUID))
        return -ENOMEM;
    driver_data =
        devm_kzalloc(&wdevv->dev, sizeof(struct driver_data_t), GFP_KERNEL);
    dev_set_drvdata(&wdevv->dev,driver_data );
    w_dev = wdevv;
    //Unlock the fan speeds
    wmi_eval_int_method(14,7681 );
    wmi_eval_int_method(14,1638410 );
    //Set fan speeds to 512
    wmi_eval_int_method(16,5121 );
    wmi_eval_int_method(16,5124 );
    dy_kbbacklight_set(1, 5, 100, 1, 255, 0, 0);
    return 0;
}
//Wmi Functions
extern void __wmi_eval_method(struct wmi_device * wdev,int methodid ,int instance ,struct acpi_buffer *inbuffer){
    struct acpi_buffer out = {ACPI_ALLOCATE_BUFFER, NULL};
    wmidev_evaluate_method(wdev,instance ,methodid ,inbuffer ,&out );
}
extern void wmi_eval_method(int methodid,struct acpi_buffer inputacpi){
    __wmi_eval_method(w_dev,methodid ,0 ,&inputacpi );
}
extern void wmi_eval_int_method(int methodid,int input){
    struct acpi_buffer in = {(acpi_size)sizeof(input),&input};
    wmi_eval_method(methodid,in );
}
//Concatenate Function*
//Thanks to: https://stackoverflow.com/questions/12700497/how-to-concatenate-two-integers-in-c

unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;
}
//Set Fan Speeds

extern int fan_set_speed(int speed ,int fan ){
    int merged=  concatenate(speed,fan);
    printk(KERN_INFO"%d",merged);
    wmi_eval_int_method(16,merged );
    return 0;
}
//Keyboard RGB Led
extern void dy_kbbacklight_set(int mode, int speed, int brg, int drc, int red, int green, int blue){
    u8 dynarray [16] = {mode, speed, brg, 0, drc, red, green, blue, 0, 1, 0, 0, 0, 0, 0, 0};
    struct acpi_buffer in  = {(acpi_size)sizeof(dynarray),dynarray};
    wmi_eval_method(20,in);
}
static int cdev_create(const char *name, int major, int minor, struct class *cls, struct device **dev) {
    *dev = device_create(cls, NULL, MKDEV(major, minor), NULL, name);
    return IS_ERR(*dev) ? PTR_ERR(*dev) : 0;
}

static int __init module_startup(void) {
    int status;
    
    if (!wmi_has_guid(WMI_GAMING_GUID)) {
        printk(KERN_ERR "WMI GUID not found");
        return -ENODEV;
    }

    if (alloc_chrdev_region(&cdev, 0, 2, "acernitrogaming")) {
        printk(KERN_ERR "Failed to allocate chrdev");
        return -ENXIO;
    }

    cmajor = MAJOR(cdev);
    cclass = class_create("acernitrogaming");
    if (IS_ERR(cclass)) {
        status = PTR_ERR(cclass);
        goto chrdev_cleanup;
    }

    if ((status = cdev_create("fan1", cmajor, 0, cclass, &dev1)) ||
        (status = cdev_create("fan2", cmajor, 1, cclass, &dev2))) {
        goto class_cleanup;
    }

    if ((status = wmi_driver_register(&wdrv))) {
        printk(KERN_ERR "WMI registration failed %d", status);
        goto devices_cleanup;
    }

    return 0;

devices_cleanup:
    if (dev1) device_destroy(cclass, MKDEV(cmajor, 0));
    if (dev2) device_destroy(cclass, MKDEV(cmajor, 1));
class_cleanup:
    class_destroy(cclass);
chrdev_cleanup:
    unregister_chrdev_region(cdev, 2);
    return status;
}

static void __exit module_finish(void) {
    wmi_driver_unregister(&wdrv);
    device_destroy(cclass, MKDEV(cmajor, 0));
    device_destroy(cclass, MKDEV(cmajor, 1));
    class_destroy(cclass);
    unregister_chrdev_region(cdev, 2);
    printk(KERN_INFO "Driver unloaded");
}

module_init(module_startup);
module_exit(module_finish);
