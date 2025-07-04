#include "nativefilter.h"
#include "qcoreapplication.h"
#include "windows.h"

NativeFilter::NativeFilter(QObject *parent)
{

}

bool NativeFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if(msg->message==WM_HOTKEY||msg->wParam==1){
            qDebug()<<"截屏热键点击";
        }
    }
    return false;
}
