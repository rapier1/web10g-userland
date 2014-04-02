#include "conn_tablemodel.hpp"
#include <QSet>
#include <QDebug>

ConnTableModel::ConnTableModel(QObject *parent, estats_nl_client *nl_client)
    : EstatsTableModel(parent, nl_client)
{
    initialize();
}

Qt::ItemFlags ConnTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QStandardItemModel::flags(index);
    if (index.isValid())
        theFlags = Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    return theFlags;
}

void ConnTableModel::initialize()
{
    setHorizontalHeaderLabels(QStringList() << tr("CID")
        << tr("Cmdline") << tr("PID") << tr("UID")
        << tr("LocalAddress") << tr("LocalPort")
        << tr("RemAddress") << tr("RemPort"));

    prev_cids = QSet<quint32>();
}

void ConnTableModel::clear()
{
    QStandardItemModel::clear();
    initialize();
}

void ConnTableModel::update()
{
    struct estats_connection_list* connlist = NULL;
//    struct list_head* ci_head = NULL;
//    struct estats_list* ci_pos;
    struct estats_connection_info* ci;

    quint32 qucid;

    QMap<quint32, QList<QStandardItem*> > connectionInfo;

    QSet<quint32> curr_cids = QSet<quint32>();

    estats::Check(estats_connection_list_new(&connlist));
    estats::Check(estats_list_conns(connlist, nl_client));
    estats::Check(estats_connection_list_add_info(connlist));

//    ci_head = &connlist->connection_info_head;

//    ESTATS_LIST_FOREACH(ci_pos, ci_head) {
    estats_list_for_each(&connlist->connection_info_head, ci, list) {
        int cid, pid, uid;
        char* cmdline;
        struct estats_connection_tuple tuple;
        struct estats_connection_tuple_ascii tuple_ascii;
//	struct estats_connection_info* conninfo = ESTATS_LIST_ENTRY(ci_pos, estats_connection_info, list);

        estats::Check(estats_connection_info_get_cid(&cid, ci));

        qucid = (quint32)cid;

        curr_cids.insert(qucid);

        if (prev_cids.contains(qucid)) continue;

        estats::Check(estats_connection_info_get_pid(&pid, ci));
        estats::Check(estats_connection_info_get_uid(&uid, ci));

        estats::Check(estats_connection_info_get_cmdline(&cmdline, ci));
        estats::Check(estats_connection_info_get_tuple(&tuple, ci));
        estats::Check(estats_connection_tuple_as_strings(&tuple_ascii, &tuple));

        QList<QStandardItem*> items;

        QStandardItem *cidItem = new QStandardItem(QString::number(cid));
        cidItem->setData(cid, Qt::DisplayRole);
        items << cidItem;

        items << new QStandardItem(cmdline);

        items << new QStandardItem(QString::number(pid));
        items << new QStandardItem(QString::number(uid));
        items << new QStandardItem(tuple_ascii.local_addr);
        items << new QStandardItem(tuple_ascii.local_port);
        items << new QStandardItem(tuple_ascii.rem_addr);
        items << new QStandardItem(tuple_ascii.rem_port);

        connectionInfo[qucid] = items;

        free((void*) cmdline);
    }

    estats_connection_list_free(&connlist);

    QSet<quint32> stale_cids = prev_cids - curr_cids;

    QSet<quint32> new_cids = curr_cids - prev_cids;

    // remove stale
    QSetIterator<quint32> j(stale_cids);
    while (j.hasNext()) {
        QList<QStandardItem *> list = findItems(QString::number(j.next()));
        QModelIndex tmpIndex = indexFromItem(list[0]);

        if (tmpIndex.isValid())
            removeRow(tmpIndex.row());
    }

    // add new
    QSetIterator<quint32> i(new_cids);
    while (i.hasNext())
        insertRow(0, connectionInfo[i.next()]);

    prev_cids = curr_cids;
}





