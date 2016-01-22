#ifndef DISPLAYKEYBOARD_H
#define DISPLAYKEYBOARD_H

#include <QObject>
#include <QVariant>
#include <QProcess>
#include <QTextStream>

#include <QDebug>

class DisplayKeyboard : public QObject
{
    Q_OBJECT
    public:
        explicit DisplayKeyboard(QObject *parent = 0);
        ~DisplayKeyboard();

    public slots:
        void textFieldFocus(QVariant field);
        void textFieldBlur(QVariant field);
        void registerTextField(QObject* field);

    private:
        qint64 processId;
        QProcess* keyboard;

};

#endif // DISPLAYKEYBOARD_H
