#ifndef HIGHPRECISIONTIMECOUNTER
#define HIGHPRECISIONTIMECOUNTER

#include <sys/time.h>
#include <vector>
#include <stdio.h>

class highPrecisionTimeCounter
{
public:
    highPrecisionTimeCounter() { }

    inline void initCounter();
    inline void mark();

    inline void printDiffTimeUs();

private:
    inline int difTime(const struct timeval &tm2, const struct timeval &tm1);

private:
    std::vector<struct timeval> vectorTm;
};

#endif // HIGHPRECISIONTIMECOUNTER



inline void highPrecisionTimeCounter::initCounter()
{
    vectorTm.clear();
    struct timeval tm;
    gettimeofday(&tm, 0);
    vectorTm.push_back(tm);
}

inline void highPrecisionTimeCounter::mark()
{
    struct timeval tm;
    gettimeofday(&tm, 0);
    vectorTm.push_back(tm);
}

inline void highPrecisionTimeCounter::printDiffTimeUs()
{
    int size = vectorTm.size();
    if (size < 2) {
        return ;
    }

    printf("-----------------------------------\n");
    for(int i = 1; i < size; i++)
        printf("difTm of mark%d~mark%d: %dus\n", i, i-1, difTime(vectorTm[i], vectorTm[i-1]));
    int t = difTime(vectorTm[size-1], vectorTm[0]);
    printf("sum of difTm: %dms.%dus\n", t/1000, t%1000);
    printf("-----------------------------------\n");

}

inline int highPrecisionTimeCounter::difTime(const timeval &tm2, const timeval &tm1)
{
    int t = tm2.tv_sec-tm1.tv_sec;
    t *= 1000000;
    t += tm2.tv_usec-tm1.tv_usec;

    return t;
}
