#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include "../libdsp-x15/library.h"

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent = 0, uint32_t bufSize = 512);
    ~AudioProcessor();
    void setFilterCoefficients(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain);
    static std::vector<float> calcBiquadCoefficients(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain);

private:
    float _b[3], _a[2];
    bool _filterIsReady;
    uint32_t _bufSize;
    static AudioProcessor *_instance;
    DspTaskFactory& _dspTaskFactory;
    TaskProcessor& _taskProcessor;
    FilterBiquadSP* _filter;

    static void _callbackDSP(DspTask& task);

signals:
    void audioDataReady(float *processData);
    void spectrumDataReady(float *plotData);

public slots:
    void processData(float *data);
};

#endif // AUDIOPROCESSOR_H
