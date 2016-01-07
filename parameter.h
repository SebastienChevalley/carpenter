#ifndef PARAMETER_H
#define PARAMETER_H

#include <QSharedPointer>

/**
 * @brief The Parameter class
 *
 * It is used in the unification part of the
 * constraint solver to be able to replace
 * the binding of the parameter value by
 * another (what QSharedPointer<double> didn't
 * offered)
 */
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
