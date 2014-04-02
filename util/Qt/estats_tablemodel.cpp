#include "estats_tablemodel.hpp"

EstatsTableModel::EstatsTableModel(QObject *parent, estats_nl_client *nl_client)
    : QStandardItemModel(parent)
{
    this->nl_client = nl_client;
}

