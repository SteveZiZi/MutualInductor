#include "eeprom.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

eeprom::eeprom()
    :fd(-1)
{

}

eeprom::~eeprom()
{
    close_dev();
}


int eeprom::open_dev(const char *dev)
{
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        printf("%s: open %s failed: %s\n", __func__, dev, strerror(errno));
        return -ENODEV;
    }
    return fd;
}

int eeprom::close_dev()
{
    if (fd >= 0)
        return close(fd);
    return -1;
}

int eeprom::read_dev(int length, void *data)
{
    if (fd >= 0) {
        lseek(fd, 0, SEEK_SET);
        return read(fd, data, length);
    }
    return -1;
}

int eeprom::write_dev(int length, void *data)
{
    if (fd >= 0) {
        lseek(fd, 0, SEEK_SET);
        return write(fd, data, length);
    }
    return -1;
}



