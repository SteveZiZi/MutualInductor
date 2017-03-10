

#include <math.h>
#include <assert.h>
#include <iostream>
#include <string.h>

#include "raw_wave.hpp"
#include "fftw3.h"


// fftw_complex *in, *out;
// fftw_plan p;
// ...
// in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
// out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
// // 输入数据in赋值
//     fftw_plan_dft_r2c_1d(n, in, out, flags);
// p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
// fftw_execute(p); // 执行变换
// ...
// fftw_destroy_plan(p);
// fftw_free(in);
// fftw_free(out);

class fftw_calculator
{
    fftw_plan plan;
    int np;
    double *in;
    fftw_complex *out;

    void init( void ) {
        if ( plan ){
            fftw_destroy_plan(plan);
        }
        if (in){
            fftw_free(in);
        }
        if (out){
            fftw_free(out);
        }
        in = (double*)fftw_malloc( np * sizeof(in[0]) );
        out = (fftw_complex*)fftw_malloc( np * sizeof(out[0]));
        plan = fftw_plan_dft_r2c_1d( np, in, out, FFTW_ESTIMATE );
        assert(in);
        assert(out);
        assert(plan);
    }

public:

    void set_np( int np ) {
        if ( np == this->np )
        {
            return ;
        }
        this->np = np;
        init();
    }

    int get_np() const {return np;}

    void fft( const std::vector<double> &in, std::vector<std::complex<double> > &out) {
        memcpy( this->in, &in[0], sizeof(this->in[0]) * this->np );
        fftw_execute( this->plan );
        out.resize( np );
        memcpy( &out[0], this->out, sizeof(this->out[0]) * np );
    }

    fftw_calculator():plan(0),np(0),in(0),out(0) {
    }

    virtual ~fftw_calculator() {
        if ( plan ){
            fftw_destroy_plan( plan );
            plan = 0;
        }
    }
};


static fftw_calculator  g_fft;

raw_wave::raw_wave():
    channel_id(0),
    positive_peak(0.0),
    negative_peak(0.0),
    np(0),
    bCalcRms(false),
    bCalcFFT(false),
    bCalcPeak(false),
    zero_offset(0.0)
{
}


raw_wave &raw_wave::operator = ( const raw_wave &other )
{
    this->channel_id = other.channel_id;
    this->fft_out = other.fft_out;
    this->harm = other.harm;
    this->angle = other.angle;
    this->positive_peak = other.positive_peak;
    this->negative_peak = other.negative_peak;

    this->freq = other.freq;
    this->fs = other.fs;
    this->np = other.np;
    this->np_ncycled = other.np_ncycled;
    this->np_per_cycle = other.np_per_cycle;
    this->raw_data = other.raw_data;
    this->rms = other.rms;
    this->thd = other.thd;

    this->bCalcRms = other.bCalcRms;
    this->bCalcFFT = other.bCalcFFT;
    this->bCalcPeak = other.bCalcPeak;
    this->zero_offset = other.zero_offset;
    return *this;
}

raw_wave::~raw_wave( )
{
}

void raw_wave::calculate_rms( void )
{
    if (bCalcRms)
        return;

    double sum = 0;
    for(std::vector<double>::iterator it=raw_data.begin(); it!=raw_data.end(); it++) {
        sum += *it * *it;
    }
    this->rms = sqrt( sum/raw_data.size() );
    this->rms -= this->zero_offset;
    bCalcRms = true;
}

void raw_wave::calculate_peak( void )
{
    if (bCalcPeak)
        return;

    double t;
    int np = this->raw_data.size();
    double max = this->raw_data[0];
    double min = this->raw_data[0];

    for (int i=1; i<np; i++)
    {
        t = this->raw_data[i];
        if ( t > max )
            max = t;
        if (t < min)
            min = t;
    }

    positive_peak = max;
    negative_peak = min;
    bCalcPeak = true;
}

double raw_wave::calculate_freq( void )
{
    return freq;
}

void raw_wave::calculate_all( void )
{
    this->calculate_rms();
    this->fft();
    this->calculate_thd();
    this->calculate_peak();
}

void raw_wave::fft( void )
{
    if (bCalcFFT)
        return ;

    unsigned int n = 0;

    assert( this->get_np() );
    g_fft.set_np( this->get_np() );
    g_fft.fft( this->raw_data, this->fft_out);

    n = this->get_np();
    assert( n == this->fft_out.size() );
    this->harm.resize( this->get_np() );
    this->angle.resize( this->get_np() );
    std::vector<double> &h = this->harm;
    std::vector<double> &a = this->angle;
    std::vector<std::complex<double> > &o = this->fft_out;
    h[0] = std::abs( o[0] ) / (n);
    a[0] = 0;
    double t = 0;
    for (unsigned int i=1; i<n; i++ ){
        h[i] = std::abs( o[i] ) / (n/2.0) * M_SQRT1_2;
        t = std::arg( o[i] ) + M_PI_2;
        if ( t >= M_PI ) {
            t -= M_PI*2;
        }
        a[i] = t;
    }
    bCalcFFT = true;
}

int raw_wave::get_harm_index( double freq ) const
{
    double delta_f = (double)fs / np;
    int index = (int)(freq / delta_f);
    assert( index >= 0 && index < np );
    return index;
}

double raw_wave::get_thd( void ) const
{
    return this->thd;
}

double raw_wave::calculate_thd( void )
{
    if ( harm.size() < 2 )
    {
        return 0.0;
    }

    double base = get_base_rms();
    double rms = get_rms();
    double t = rms*rms - base*base;
    double harm_rms = 0.0;
    if ( t > 0.0 )
    {
        harm_rms = sqrt( t );
    } else {
        harm_rms = 0.0;
    }

    this->thd = harm_rms / base;
    return this->thd;
}

double raw_wave::get_base_rms( void ) const
{
    return this->harm[ this->get_harm_index(this->freq) ];
}

double raw_wave::get_rms( double freq ) const
{
    return this->harm[ this->get_harm_index(freq) ];
}

double raw_wave::get_base_angle( void ) const
{
    return this->angle[ this->get_harm_index(this->freq) ];
}

double raw_wave::get_angle( double freq ) const
{
    return this->angle[ this->get_harm_index(freq) ];
}

double raw_wave::get_multiply( const raw_wave &other, int np ) const
{
    double sum = 0;

    for (int i=0; i<np; i++)
    {
        sum += this->raw_data[i] * other.raw_data[i];
    }
    return sum;
}

double raw_wave::get_multiply( const raw_wave &other ) const
{
    double sum = 0;
    int np1 = this->raw_data.size();
    int np2 = other.raw_data.size();
    assert( np1 == np2 );
    int np = np1 > np2 ? np2:np1;
    for (int i=0; i<np; i++)
    {
        sum += this->raw_data[i] * other.raw_data[i];
    }
    return sum;
}


double raw_wave::get_peek( void ) const
{    
    double positive_peak = fabs(this->positive_peak);
    double negative_peak = fabs(this->negative_peak);

    return (positive_peak > negative_peak) ? positive_peak : negative_peak;
}

void raw_wave::set_raw_data( double *v, unsigned int n, double zero_offset)
{
    std::vector<double> &to = this->get_writable_buffer();

    assert( n <= to.size() );

    for (unsigned int i=0; i<n; i++) {
        to[i] = v[i];
    }

    bCalcFFT = false;
    bCalcPeak = false;
    bCalcRms = false;
    this->zero_offset = zero_offset;
}

void raw_wave::set_raw_data( double *v, unsigned int n, double fs, double zero_offset )
{
    this->set_dynamic( n, fs );
    set_raw_data( v, n );
    bCalcFFT = false;
    bCalcPeak = false;
    bCalcRms = false;
    this->zero_offset = zero_offset;
}

void raw_wave::set_raw_data( int16_t *raw16,
                             int offset,
                             unsigned int n,
                             double scale,
                             double line_offset,
                             double zero_offset)
{
    std::vector<double> &raw = this->get_writable_buffer();

    // assert( n == this->get_np() );
    for (unsigned int i=0; i<n; i++) {
        raw[i+offset] = raw16[i] * scale+line_offset;
    }
    bCalcFFT = false;
    bCalcPeak = false;
    bCalcRms = false;
    this->zero_offset = zero_offset;
}

void raw_wave::set_raw_data( int16_t *raw16,
                             unsigned int n,
                             double scale,
                             double line_offset,
                             double zero_offset)
{
    std::vector<double> &raw = this->get_writable_buffer();

    assert( n == this->get_np() );
    for (unsigned int i=0; i<n; i++) {
        raw[i] = raw16[i] * scale+line_offset;
    }
    bCalcFFT = false;
    bCalcPeak = false;
    bCalcRms = false;
    this->zero_offset = zero_offset;
}
