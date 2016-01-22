#include "displaykeyboard.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#include <objbase.h>
#include <Tchar.h>
#endif

DisplayKeyboard::DisplayKeyboard(QObject *parent) : QObject(parent)
{
    keyboard = new QProcess(this);
}

DisplayKeyboard::~DisplayKeyboard() {
    delete keyboard;
}

void DisplayKeyboard::textFieldFocus(QVariant field) {
    qDebug() << "focus" << field;

#ifdef Q_OS_WIN
    HWND wKB = FindWindow(TEXT("IPTip_Main_Window"), NULL);

    if (wKB != NULL) {
        ShellExecuteA(NULL, "open", "\"C:\\Program Files\\Common Files\\microsoft shared\\ink\\TabTip.exe\"", "", 0, SW_NORMAL);
    }
#endif
}

void DisplayKeyboard::textFieldBlur(QVariant field) {
    qDebug() << "blur" << field << processId;

#ifdef Q_OS_WIN
    HWND wKB = FindWindow(TEXT("IPTip_Main_Window"), NULL);
    if (wKB != NULL && IsWindowVisible(wKB)) {
        PostMessage(wKB, WM_SYSCOMMAND, SC_CLOSE, 0);
    }
#endif
}

void DisplayKeyboard::registerTextField(QObject *field) {
    qDebug() << "register" << field;

    field->connect(field, SIGNAL(gotFocus(QVariant)), this, SLOT(textFieldFocus(QVariant)));
    field->connect(field, SIGNAL(lostFocus(QVariant)), this, SLOT(textFieldBlur(QVariant)));
}
