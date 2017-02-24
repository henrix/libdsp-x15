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
    API *_api;
    int _bufSize;
    static AudioProcessor *_instance;

    static void _callbackDSP(CallbackResponse *clbkRes);

signals:
    void processedDataReady(float *data);

public slots:
    void processData(float *data);
};

#endif // AUDIOPROCESSOR_H
