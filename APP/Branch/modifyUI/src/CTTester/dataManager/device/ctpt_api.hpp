#ifndef __CTPT_API_H
#define __CTPT_API_H

#include <string>

#include "ctpt_def.h"



namespace CTPT {

class ctpt_ctrl_p;
class ctpt_ctrl
{
public:
    CPTAPI ctpt_ctrl();
    CPTAPI virtual ~ctpt_ctrl();

public:

    // init, reinit
    CPTAPI int init( void );
    CPTAPI int reinit( void );

    CPTAPI std::string fpga_ver(void);
    CPTAPI std::string ctpt_ver(void);

    // config
    CPTAPI int config( const struct ct_setting & ); // 配置CT
    CPTAPI int config( const struct pt_setting & ); // 配置PT
    CPTAPI int config( double voltage,
                       double current,
                       double freq,
                       user_test_mode testMode = USER_TEST_SELFCHECK); // 用户自测配置

    // 开始试验
    CPTAPI int start( void );

    // 结束
    CPTAPI int stop( void );

    // 获取测试结果
    CPTAPI int get_ctpt_result(ctpt_result &out);


    // 获取 用户自测结果
    // return 0 成功，无错误
    // return -1, 测试失败
    CPTAPI int get_user_test_result( double *voltage,
                                     double *current,
                                     double *freq_measured = 0);

private:
    ctpt_ctrl_p *_private;
};

}


#endif /* __CTPT_API_H */

