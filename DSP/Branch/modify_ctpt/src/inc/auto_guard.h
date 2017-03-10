#ifndef M_LIB_AUTO_GUARD_H
#define M_LIB_AUTO_GUARD_H

template<class T>                                             
class CAutoGuard                                              
{                                                             
public:                                                       
    CAutoGuard(T &mtx) : m_mtx(mtx)                       
    {                                                     
        m_mtx.lock();                                 
    }                                                     
    ~CAutoGuard()                                         
    {                                                     
        m_mtx.unlock();                               
    }                                                     
protected:                                                    
        T &m_mtx;                                             
};                                                            
                                                              
#define AUTO_GUARD( guard_tmp_var, MUTEX_TYPE, mtx ) \
        CAutoGuard<MUTEX_TYPE> guard_tmp_var(mtx)   
        
#endif
