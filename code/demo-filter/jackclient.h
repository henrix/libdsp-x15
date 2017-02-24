#ifndef JACKCLIENT_H
#define JACKCLIENT_H

#include <QtCore/QObject>
#include <jack/jack.h>
#include <functional>
#include <vector>

class JackClient : public QObject
{
    Q_OBJECT

public:
    explicit JackClient(QObject *parent = 0);
    void start();
    void stop();
    int getBufSize();
    void setBufSize(int size);
    int getRate();

private:
    jack_client_t *_client;
    jack_port_t *_in, *_out;
    bool _processedDataIsReady;
    float *_outBufPtr;

    static int _jackStaticCallback(jack_nframes_t nframes, void *arg);
    int _jackCallback(jack_nframes_t nframes);

signals:
    void dataReady(float *data);

public slots:
    void writeData(float *data);
};

#endif // JACKCLIENT_H
