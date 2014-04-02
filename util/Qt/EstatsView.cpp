#include "EstatsView.hpp"

bool EApplication::notify(QObject * receiver, QEvent * event) {
    try {
        return QApplication::notify(receiver, event);
    } catch(estats::Error& e) {
        qCritical() << "Exception thrown:" << e.what();
    }
    return false;
}

namespace estats {

Error::Error(estats_error *error) throw()
{
    this->error = error;
    message = estats_error_get_message(error);
}

Error::~Error() throw()
{
    estats_error_free(&error);
}

Check::Check(estats_error *err)
{
    if (err != NULL) throw Error(err);
}

} // namespace estats
