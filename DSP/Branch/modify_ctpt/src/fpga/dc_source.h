#ifndef DC_SOURCE_H
#define DC_SOURCE_H

#include <stdint.h>

class dc_source
{
public:
    dc_source();

    int startOutput();
    int stopOutput();
    bool outputStarted() { return outputStarted_; }

    int safe_output(double v);

    int demagnetization();

private:
    int outputVol(double v);

private:
    bool outputStarted_;
    double  output_vol;
};

#endif // DC_SOURCE_H
