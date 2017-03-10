#include "adcCalibration.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define DEV_A_NODE   "/sys/class/i2c-dev/i2c-1/device/1-0050/eeprom"
#define DEV_B_NODE   "/sys/class/i2c-dev/i2c-1/device/1-0052/eeprom"

#define version_calibration 0x100

struct eeprom_content {
    uint16_t version;              // version of calibration program
    uint16_t crcSum;               // sum of crc, from num_of_ad to result[num_of_ad-1]
    uint32_t num_of_ad;            // epprom include the number of ad channel
    ad_calibrate_result result[1]; // ad channel calibrate result
};

class calibrate_dev
{
    friend class CAdcCalibration;
public:
    calibrate_dev();
    ~calibrate_dev();

    bool init(const char *dev, int num_of_ad);

private:
    inline int open_dev(const char *dev);
    inline int close_dev();
    inline int read_dev(int length, void *data);
    inline int write_dev(int length, void *data);

    uint16_t calc_crc(eeprom_content* content);
    int verify_content(eeprom_content* content, uint16_t num_of_ad);

private:
    int fd;
    bool isBadRom;
    bool isValid;
    uint8_t *content_eeprom;
};

calibrate_dev::calibrate_dev() :
    fd(-1),
    isBadRom(true),
    isValid(false)
{
    content_eeprom = new uint8_t[256];
}

calibrate_dev::~calibrate_dev()
{
    close_dev();
    delete[] content_eeprom;
}

bool calibrate_dev::init(const char *dev, int num_of_ad)
{
    if (dev == NULL)
        return false;

    int ret = open_dev(dev);
    if (ret < 0) {
        printf("Open dev:%s failed\n", dev);
        return false;
    }

    if (read_dev(256, content_eeprom)<0) {
        printf("Read dev:%s failed\n", dev);
        return false;
    }

    isBadRom = false;
    ret = verify_content(reinterpret_cast<eeprom_content*>(content_eeprom), num_of_ad);
    if (ret < 0) {
        printf("verify_content dev:%s failed(errCode:%d).\n", dev, ret);
        return false;
    }
    isValid = true;

    return true;
}

uint16_t calibrate_dev::calc_crc(eeprom_content *content)
{
    int size = sizeof(content->num_of_ad);

    uint8_t* p = reinterpret_cast<uint8_t*>(&content->result[0]);
    for(uint16_t chnl=0; chnl<content->num_of_ad; chnl++) {
        ad_calibrate_result* result_ = reinterpret_cast<ad_calibrate_result*>(p);
        p += sizeof(result_->numOfRange) + sizeof(range_factor)*result_->numOfRange;
        size += sizeof(result_->numOfRange) + sizeof(range_factor)*result_->numOfRange;
    }
    int sum = 0;
    uint8_t *tmp = reinterpret_cast<uint8_t*>(&content->num_of_ad);
    for(int n=0; n<size; n++) {
        sum += tmp[n];
    }

    if (sum > 0xFFFF) {
        sum += 1;
        sum = ~sum;
    }
    return (uint16_t)(sum&0xFFFF);
}

int calibrate_dev::verify_content(eeprom_content *content, uint16_t num_of_ad)
{
    if (content == NULL) {
        return -1;
    }

    if (content->version != version_calibration) {
        return -2;
    }

    if (content->num_of_ad != num_of_ad) {
        return -3;
    }

    if (calc_crc(content) != content->crcSum) {
        return -4;
    }

    return 0;
}

int calibrate_dev::open_dev(const char *dev)
{
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        printf("%s: open %s failed: %s\n", __func__, dev, strerror(errno));
        return -ENODEV;
    }
    return fd;
}

int calibrate_dev::close_dev()
{
    if (fd >= 0)
        return close(fd);
    return -1;
}

int calibrate_dev::read_dev(int length, void *data) {
    if (fd >= 0) {
        lseek(fd, 0, SEEK_SET);
        return read(fd, data, length);
    }
    return -1;
}

int calibrate_dev::write_dev(int length, void *data) {
    if (fd >= 0) {
        lseek(fd, 0, SEEK_SET);
        return write(fd, data, length);
    }
    return -1;
}


/*************************************************************************
 *     class CAdcCalibration
 * ***********************************************************************/
CAdcCalibration::CAdcCalibration()
{
    m_dev[0] = new calibrate_dev();
    m_dev[1] = new calibrate_dev();

    m_resultChanged[0] = false;
    m_resultChanged[1] = false;
}

CAdcCalibration::~CAdcCalibration()
{
    delete m_dev[0];
    delete m_dev[1];
}

CAdcCalibration *CAdcCalibration::getInstance()
{
    static CAdcCalibration calibration;

    return &calibration;
}

int CAdcCalibration::init()
{
    int ret = 0;
    if (!m_dev[0]->init(DEV_A_NODE, 3)) {
        m_resultChanged[0] = true;
        ret = -1;
    }
    format_content1();

    if (!m_dev[1]->init(DEV_B_NODE, 1)) {
        m_resultChanged[1] = true;
        ret = -1;
    }
    format_content2();

    return ret;
}

bool CAdcCalibration::chnl_is_valid(int chnl)
{
    if( chnl==LOAD_CUR_CHNL || chnl==SRC_VOL_CHNL || chnl==LOAD_VOL_CHNL )
        return m_dev[0]->isValid;
    else if (chnl == PRI_VOL_CHNL)
        return m_dev[1]->isValid;

    return false;
}

const ad_calibrate_result *CAdcCalibration::result(int chnl)
{
    if (chnl < MAX_CHNL_NUM) {
        return adCalibrateResult[chnl];
    }
    return NULL;
}

int CAdcCalibration::setResult(int chnl, const ad_calibrate_result &result_)
{
    if( chnl==LOAD_CUR_CHNL || chnl==SRC_VOL_CHNL || chnl==LOAD_VOL_CHNL )
        m_resultChanged[0] = true;
    else if (chnl == PRI_VOL_CHNL)
        m_resultChanged[1] = true;
    else
        return -1;

    if (adCalibrateResult[chnl]->numOfRange == result_.numOfRange) {
        memcpy(adCalibrateResult[chnl]->factors, result_.factors, sizeof(range_factor)*result_.numOfRange);
        return 0;
    }

    return -1;
}

ad_calibrate_result *CAdcCalibration::get_calibrate_result(int chnl)
{
    return adCalibrateResult[chnl];
}

void CAdcCalibration::saveResult()
{
    if (m_resultChanged[0] && !m_dev[0]->isBadRom)
    {
        printf("CAdcCalibration::saveResult 1++\n");
        eeprom_content * content = reinterpret_cast<eeprom_content*>(m_dev[0]->content_eeprom);
        content->crcSum = m_dev[0]->calc_crc(content);
        int ret = m_dev[0]->write_dev(256, m_dev[0]->content_eeprom);
        if (ret != 256)
            ret = m_dev[0]->write_dev(256, m_dev[0]->content_eeprom);
        printf("CAdcCalibration::saveResult 1 ret(%d)\n", ret);
    }

    if (m_resultChanged[1] && !m_dev[1]->isBadRom)
    {
        printf("CAdcCalibration::saveResult 2\n");
        eeprom_content * content = reinterpret_cast<eeprom_content*>(m_dev[1]->content_eeprom);
        content->crcSum = m_dev[1]->calc_crc(content);
        int ret = m_dev[1]->write_dev(256, m_dev[1]->content_eeprom);
        if (ret != 256)
            m_dev[1]->write_dev(256, m_dev[1]->content_eeprom);
        printf("CAdcCalibration::saveResult 2 ret(%d)\n", ret);
    }
}

void CAdcCalibration::format_content1()
{
    eeprom_content * content = reinterpret_cast<eeprom_content*>(m_dev[0]->content_eeprom);
    content->version = version_calibration;
    content->num_of_ad = 3;
    content->crcSum = 0;

    uint8_t* p = reinterpret_cast<uint8_t*>(&content->result[0]);
    for(int chnl=0; chnl<3; chnl++) {
        adCalibrateResult[chnl] = reinterpret_cast<ad_calibrate_result*>(p);
        adCalibrateResult[chnl]->numOfRange = 4;
        p += sizeof(adCalibrateResult[chnl]->numOfRange) + sizeof(range_factor)*4;
    }
}

void CAdcCalibration::format_content2()
{
    eeprom_content * content = reinterpret_cast<eeprom_content*>(m_dev[1]->content_eeprom);
    content->version = version_calibration;
    content->num_of_ad = 1;
    content->crcSum = 0;

    adCalibrateResult[3] = &content->result[0];
    adCalibrateResult[3]->numOfRange = 4;
}

void CAdcCalibration::dumpRange()
{
    for(int chnl=0; chnl<4; chnl++) {
        printf("========== chnl:%d ===============\n", chnl);
        for(uint16_t range=0; range<adCalibrateResult[chnl]->numOfRange; range++) {
            printf("factor:%f \t %f\n", adCalibrateResult[chnl]->factors[range].k,
                                        adCalibrateResult[chnl]->factors[range].b);
        }
        printf("\n");
    }

}


