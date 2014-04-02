#include "conn_tablemodel.hpp"
#include <QSet>
#include <QDebug>

ConnTableModel::ConnTableModel(QObject *parent, estats_agent *agent)
    : EstatsTableModel(parent, agent)
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
    estats_sockinfo* sinfo = NULL;
    estats_sockinfo_item* ci_head = NULL;
    estats_sockinfo_item* ci_pos;

    quint32 qucid;

    QMap<quint32, QList<QStandardItem*> > connectionInfo;

    QSet<quint32> curr_cids = QSet<quint32>();

    estats::Check(estats_sockinfo_new(&sinfo, agent));
    estats::Check(estats_sockinfo_get_list_head(&ci_head, sinfo));

    ESTATS_SOCKINFO_FOREACH(ci_pos, ci_head) {
        int cid, pid, uid;
        char* cmdline;
        struct estats_connection_spec spec;
        struct spec_ascii spec_asc;

        estats::Check(estats_sockinfo_get_cid(&cid, ci_pos));

        qucid = (quint32)cid;

        curr_cids.insert(qucid);

        if (prev_cids.contains(qucid)) continue;

        estats::Check(estats_sockinfo_get_pid(&pid, ci_pos));
        estats::Check(estats_sockinfo_get_uid(&uid, ci_pos));

        estats::Check(estats_sockinfo_get_cmdline(&cmdline, ci_pos));
        estats::Check(estats_sockinfo_get_connection_spec(&spec, ci_pos));
        estats::Check(estats_connection_spec_as_strings(&spec_asc, &spec));

        QList<QStandardItem*> items;

        QStandardItem *cidItem = new QStandardItem(QString::number(cid));
        cidItem->setData(cid, Qt::DisplayRole);
        items << cidItem;

        items << new QStandardItem(cmdline);

        items << new QStandardItem(QString::number(pid));
        items << new QStandardItem(QString::number(uid));
        items << new QStandardItem(spec_asc.src_addr);
        items << new QStandardItem(spec_asc.src_port);
        items << new QStandardItem(spec_asc.dst_addr);
        items << new QStandardItem(spec_asc.dst_port);

        connectionInfo[qucid] = items;

        free((void*) cmdline);
    }

    estats_sockinfo_free(&sinfo);

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
//        appendRow(connectionInfo[i.next()]);
        insertRow(0, connectionInfo[i.next()]);

    prev_cids = curr_cids;
}





