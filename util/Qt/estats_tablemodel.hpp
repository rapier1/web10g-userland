#ifndef ESTATSTABLEMODEL_HPP
#define ESTATSTABLEMODEL_HPP

#include "EstatsView.hpp"
#include <QStandardItemModel>

class EstatsTableModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit EstatsTableModel(QObject *parent=0, estats_nl_client *nl_client=NULL);

protected:
    estats_nl_client *nl_client;

private:

};

#endif // ESTATSTABLEMODEL_HPP
