#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include "../libdsp-x15/library.h"

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent = 0, int bufSize = 512);
    ~AudioProcessor();

private:
    int _bufSize;
    static AudioProcessor *_instance;
    DspTaskFactory& _dspTaskFactory;
    TaskProcessor& _taskProcessor;
    FFT_SP* _fftTask;

    static void _callbackDSP(DspTask& task);

signals:
    void processedDataReady(float *data);

public slots:
    void processData(float *data);
};

#endif // AUDIOPROCESSOR_H
