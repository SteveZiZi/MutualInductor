
#ifndef RAW_WAVE_H
#define RAW_WAVE_H

#include <vector>
#include <complex>


#include <stdint.h>

class raw_wave
{
public:
    raw_wave();

    void set_dynamic( int np, double fs ) { this->raw_data.resize( np); this->np = np; this->fs = fs;}
    std::vector<double> &get_writable_buffer() {return this->raw_data;};
    const std::vector<double> &get_readonly_buffer() const {return this->raw_data;};

    double get_freq( void ) const { return this->freq; }
    double get_fs( void ) const { return fs;}
    unsigned int get_np( void ) const { return np;}
    int get_np_ncycled( void ) const { return -1;} // not used
    int get_np_per_cycle( void ) const { return np_per_cycle;}
    void set_np_per_cycle(int np) {this->np_per_cycle = np;}
    void set_raw_data( double *raw, unsigned int n, double fs, double line_offset = 0.0);
    void set_raw_data( double *raw, unsigned int n, double line_offset = 0.0);
    void set_raw_data( int16_t *raw, unsigned int n, double scale, double line_offset, double zero_offset = 0.0);
    void set_raw_data( int16_t *raw, int offset, unsigned int n, double scale, double line_offset, double zero_offset  = 0.0);

    double get_rms( void ) const {return this->rms;};
    double get_integral_rms( void ) const {return this->rms * 2*3.1415926*this->freq;};
    double get_multiply( const raw_wave &other, int np ) const;
    double get_multiply( const raw_wave &other ) const;

    virtual ~raw_wave();

    void integral( double scale = 1.0 );

    double get_thd( void ) const;
    double get_base_angle( void ) const;
    double get_angle( double freq ) const;
    double get_base_rms( void ) const;
    double get_rms( double freq  ) const;
    double get_peek( void ) const;

    double get_positive_peak(void) const {return positive_peak;}
    double get_negative_peak(void) const {return negative_peak;}

    void set_channel_id( int id ) { this->channel_id = id;};
    int get_channel_id( void ) const {return this->channel_id;}

    void calculate_all( void );
    void calculate_rms( void );
    void calculate_peak(void);

public:
    raw_wave &operator *= ( double scale ); // copy
    raw_wave &operator = ( const raw_wave & ); // copy

    std::complex<double> get_w() const{return this->w;}

    void set_freq( double freq ){this->freq = freq;}

protected:
    double calculate_freq( void );
    double calculate_thd( void );
    void fft( void );
    int get_harm_index( double freq ) const;

private:
    int channel_id;
    std::vector<std::complex<double> > fft_out;
    std::vector<double> harm;
    std::vector<double> angle;
    double positive_peak;
    double negative_peak;

private:
    double freq;
    double fs;
    int np;
    int np_ncycled;
    int np_per_cycle;
    std::vector<double> raw_data;
    std::complex<double> w;
    double rms;
    double thd;

    bool bCalcRms;
    bool bCalcFFT;
    bool bCalcPeak;

    double zero_offset;   //ÁãµãµÄÆ«²îÖµ
};



#endif /* RAW_WAVE_H */
