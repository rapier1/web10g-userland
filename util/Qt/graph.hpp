#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QGraphicsItem>
#include <QSet>

class Graph : public QGraphicsItem
{
//    Q_OBJECT

public:
    explicit Graph(QRectF rectF, QGraphicsItem *parent=0);

    QRectF boundingRect() const { return m_rectF; }

    void paint(QPainter *painter,
	       const QStyleOptionGraphicsItem *option, QWidget *widget);
    void clear();
    void update_vars(qint32 var);
    void update_vals();

protected:
    QSet<int> vars;
    QMap<quint32, QList<QVariant>> vals;

private:
    void initialize();
    QRectF m_rectF;
};

#endif // GRAPH_HPP
