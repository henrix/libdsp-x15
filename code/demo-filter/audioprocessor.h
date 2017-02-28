#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QtCore/QObject>
#include "../libdsp-x15/library.h"

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent = 0, uint32_t bufSize = 512);
    ~AudioProcessor();
    void setFilterCoefficients(API::FILTER_TYPE type, float Fc, float Fs, float Q, float peakGain);
    static std::vector<float> calcBiquadCoefficients(API::FILTER_TYPE type, float Fc, float Fs, float Q, float peakGain);

private:
    float _b[3], _a[2];
    bool _filterIsReady;
    uint32_t _bufSize;
    API *_api;
    static AudioProcessor *_instance;

    static void _callbackDSP(CallbackResponse *clbkRes);

signals:
    void audioDataReady(float *processData);
    void spectrumDataReady(float *plotData);

public slots:
    void processData(float *data);
};

#endif // AUDIOPROCESSOR_H
