#ifndef CONNTABLEMODEL_HPP
#define CONNTABLEMODEL_HPP

#include "EstatsView.hpp"
#include "estats_tablemodel.hpp"
#include <QSet>

class ConnTableModel : public EstatsTableModel
{
    Q_OBJECT

public:
    explicit ConnTableModel(QObject *parent=0, estats_nl_client *nl_client=NULL);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void clear();
    void update();

private:
    void initialize();
    QSet<quint32> prev_cids;
};

#endif // CONNTABLEMODEL_HPP
