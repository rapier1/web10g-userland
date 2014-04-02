#ifndef VARTABLEMODEL_HPP
#define VARTABLEMODEL_HPP

#include "EstatsView.hpp"
#include "estats_tablemodel.hpp"
#include <QSet>

class VarTableModel : public EstatsTableModel
{
    Q_OBJECT

public:
    explicit VarTableModel(QObject *parent=0, estats_nl_client *nl_client=NULL, int cid=-1);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void clear();
    void update();

protected:
    int cid;
    
    estats_val_data* newdata;
    estats_val_data* olddata;
    estats_val_data* deldata;

    QMap<quint32, QList<QStandardItem*> > varInfo;

private:
    void initialize();
};

#endif // VARTABLEMODEL_HPP
