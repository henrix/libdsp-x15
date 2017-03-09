#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <jack/jack.h>
#include "../libdsp-x15/library.h"

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent, uint32_t bufSize, FilterBiquadSP::TYPE type,
                            float Fc, float Fs, float Q, float peakGain);
    ~AudioProcessor();
    static std::vector<float> calcBiquadCoefficients(FilterBiquadSP::TYPE type, float Fc,
                                                     float Fs, float Q, float peakGain);
    void startJack();
    unsigned int getBufSize();

private:
    float _b[3], _a[2];
    bool _filterIsReady;
    uint32_t _bufSize;
    static AudioProcessor *_instance;
    DspTaskFactory& _dspTaskFactory;
    TaskProcessor& _taskProcessor;
    FilterBiquadSP *_filter1, *_filter2;
    jack_client_t *_jackClient;
    jack_port_t *_in1, *_in2, *_out1, *_out2;

    static void _callbackDSP(DspTask& task);
    static int _callbackJackStatic(jack_nframes_t nframes, void *arg);
    int _callbackJack(jack_nframes_t nframes);

public slots:
    void setFilterParameter(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain);
};

#endif // AUDIOPROCESSOR_H
