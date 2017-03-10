#ifndef EEPROM_H
#define EEPROM_H


class eeprom
{
public:
    eeprom();
    ~eeprom();


    int open_dev(const char *dev);
    int close_dev();
    int read_dev(int length, void *data);
    int write_dev(int length, void *data);

private:
    int fd;
};

#endif // EEPROM_H
