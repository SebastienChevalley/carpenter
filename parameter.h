#ifndef PARAMETER_H
#define PARAMETER_H

#include <QSharedPointer>

class Parameter  {
public:
    Parameter(double x) {
        this->_value = QSharedPointer<double>(new double);
        *(this->_value) = x;
    }

    double value() {
        return *this->_value;
    }
    QSharedPointer<double> address() {
        return this->_value;
    }

    void setValue(QSharedPointer<double> value) {
        this->_value = value;
    }
    void setValue(double value) {
        *(this->_value) = value;
    }

    QSharedPointer<double> _value;
} ;
#endif // PARAMETER_H
