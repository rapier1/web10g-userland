#include "var_tablemodel.hpp"
#include <QSet>
#include <QDebug>
#include <QString>

VarTableModel::VarTableModel(QObject *parent, estats_nl_client *nl_client, int cid)
    : EstatsTableModel(parent, nl_client)
{
    this->cid = cid;
    this->nl_client = nl_client;

    estats::Check(estats_val_data_new(&newdata));
    estats::Check(estats_val_data_new(&olddata));
    estats::Check(estats_val_data_new(&deldata));

    initialize();
}

Qt::ItemFlags VarTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QStandardItemModel::flags(index);
    if (index.isValid())
        theFlags = Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    return theFlags;
}

void VarTableModel::initialize()
{
    setHorizontalHeaderLabels(QStringList() << tr("Name")
        << tr("Value") << tr("Delta"));

    quint32 index = 0;

    for (int i = 0; i < TOTAL_INDEX_MAX; i++) {
	const char* name;

	name = estats_var_array[i].name;

        QList<QStandardItem*> items;

	QStandardItem *nameItem = new QStandardItem(name);
	nameItem->setData(i, Qt::UserRole);

	items << nameItem;

//        items << new QStandardItem(name);
        items << new QStandardItem();
        items << new QStandardItem();

        varInfo[index++] = items;
    }

    QMapIterator<quint32, QList<QStandardItem*> > i(varInfo); 
    while (i.hasNext())
        appendRow(i.next().value());
}

void VarTableModel::clear()
{
    QStandardItemModel::clear();
    initialize();
}

void VarTableModel::update()
{
    static int first_update = 1;

    quint32 index = 0;
    estats_val_data* tmp;
 
    estats::Check(estats_read_vars(newdata, cid, nl_client));

    for (int i = 0; i < newdata->length; i++) {
	ESTATS_VAL_TYPE valtype = estats_var_array[i].valtype;
	ESTATS_TYPE type = estats_var_array[i].type;
	char* valstr;

	estats::Check(estats_val_as_string(&valstr, &newdata->val[i], valtype));

        QStandardItem* item = new QStandardItem(valstr); 
//        QStandardItem* item = new QStandardItem(newdata->val[i]); 
        setItem(index, 1, item);

        free((void*)valstr);

        if (!first_update && (type == ESTATS_TYPE_COUNTER32 || type == ESTATS_TYPE_COUNTER64)) {
            estats::Check(estats_val_data_delta(deldata, newdata, olddata));

	    estats::Check(estats_val_as_string(&valstr, &deldata->val[i], valtype));

            QStandardItem* item = new QStandardItem(valstr);
//            QStandardItem* item = new QStandardItem(QString::number(deldata->val[i]));

            setItem(index, 2, item);

            free((void*)valstr);
        }
	index++;
    }
    tmp = olddata;
    olddata = newdata;
    newdata = tmp;

    if (first_update) first_update = 0;
}





