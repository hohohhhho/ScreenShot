#ifndef NATIVEFILTER_H
#define NATIVEFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class NativeFilter:public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    NativeFilter(QObject* parent=nullptr);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)override;
signals:
    void screenShot();
};

#endif // NATIVEFILTER_H
