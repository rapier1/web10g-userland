


#include "graph.hpp"

#include <iostream>

Graph::Graph(QRectF rectF, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_rectF(rectF)
{
    initialize();
}

void Graph::update_vars(int var)
{
    vars.insert(var);
    std::cout << "Update vars\n";
    QSetIterator<int> i(vars);
    while (i.hasNext())
	std::cout << i.next() << "\n";
}

void Graph::update_vals()
{
}

void Graph::initialize()
{
}

void Graph::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option, QWidget* widget)
{
    std::cout << "In paint\n";
}
