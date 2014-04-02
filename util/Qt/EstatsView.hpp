#ifndef ESTATSVIEW_HPP
#define ESTATSVIEW_HPP

#include <QtGui>
#include <QApplication>

extern "C" {                                                                              
#include "estats/estats.h"                                                                
//#include "tcpestats/sockinfo.h"
}

class EApplication : public QApplication
{
public:
    EApplication(int& argc, char **argv)
        : QApplication(argc, argv) {}
    virtual ~EApplication() {}

    virtual bool notify(QObject * receiver, QEvent * event);
};

namespace estats {

class Error : public std::exception
{
public:
    explicit Error(estats_error *error) throw(); 
    ~Error() throw();

    const char *what() const throw() { return message; }

private:
    estats_error *error;
    const char *message;
};

class Check
{
public:
    explicit Check(estats_error *error=NULL);
};

} // namespace estats

#endif /* ESTATSVIEW_HPP */
