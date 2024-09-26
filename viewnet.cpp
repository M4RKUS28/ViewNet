#include "viewnet.h"

#include <QBrush>
#include <QPen>

ViewNet::ViewNet(Net *net, QRect rect, int neuron_size, bool show_weights)
    : net(net), size(rect), neuronSize(neuron_size)
{
    if(!net)
        return;

    neurons = new QGraphicsItemGroup(this);
    weights = new QGraphicsItemGroup(this);

    this->setRect(rect);
    for (unsigned layerNum = 0; layerNum < net->getTopology().size(); ++layerNum) {
        m_layers.push_back(ViewLayer());
        unsigned numOutput = (layerNum == net->getTopology().size() - 1) ? 0 : net->getTopology().at(layerNum + 1).neuronCount;
        for (unsigned neuronNum = 0; neuronNum < net->getTopology().at(layerNum).neuronCount + 1; ++neuronNum)    {
            m_layers.back().push_back(ViewNeuron(numOutput));
        }
    }
    this->resize(rect, neuron_size, 2, show_weights, true);
}

ViewNet::~ViewNet()
{

}

void ViewNet::setInputPraefix(const std::vector<std::string> &labels)
{
    this->praefixes = labels;
}

void ViewNet::setOutputSuffix(const std::vector<std::string> &labels)
{
    this->suffixes = labels;
}

void ViewNet::resize(QRect rect, int neuron_size, int line_size, bool show_weights, bool bias_preafix)
{
    if(!net)
        return;
    int spacer_x = ( rect.width() - m_layers.size() * neuron_size ) / ( m_layers.size() + 1);

    for(unsigned layer = 0; layer < m_layers.size(); layer++) {

        unsigned neuron_count = (m_layers.at(layer).size() - ((layer == m_layers.size() - 1) ? 1 : 0));
        int spacer_y = ( rect.height() - neuron_count * neuron_size ) / ( neuron_count + 1);

        for(unsigned neuron = 0; neuron < neuron_count; neuron++) {

            QPoint neuronPos(neuron_size * layer + spacer_x * (layer + 1),  neuron_size * neuron + spacer_y * (neuron + 1));
            m_layers.at(layer).at(neuron).neuronGItem = new QGraphicsEllipseItem(0, 0, neuron_size, neuron_size, this);
            neurons->addToGroup(m_layers.at(layer).at(neuron).neuronGItem);

            m_layers.at(layer).at(neuron).neuronGItem->setPos(neuronPos);

            for(unsigned conns = 0; conns < m_layers.at(layer).at(neuron).m_outputWeights.size(); conns++) {
                auto &con = m_layers.at(layer).at(neuron).m_outputWeights.at(conns);
                con.line = new QGraphicsLineItem(neuronPos.x() + neuron_size, neuronPos.y() + neuron_size / 2,  neuronPos.x() + spacer_x + neuron_size,
                                                 neuron_size * conns +  ( rect.height() - (m_layers.at(layer + 1).size() - ((layer + 1 == m_layers.size() - 1) ? 1 : 0)) * neuron_size ) / ( m_layers.at(layer + 1).size() - ((layer + 1 == m_layers.size() - 1) ? 1 : 0) + 1)
                                                         * (conns + 1) + neuron_size / 2, this);
                weights->addToGroup(con.line);
                con.line->setPen(QPen(QBrush(Qt::black), line_size));

                // Create a QGraphicsTextItem
                con.text_weight = new QGraphicsTextItem(QString::number( net->getConWeight(layer, neuron, conns )  ), this);
                weights->addToGroup(con.text_weight);

                // Set the position of the textItem relative to the lineItem
                QPointF textPos = con.line->boundingRect().center();
                con.text_weight->setPos(textPos);
                con.text_weight->setVisible(show_weights);

            }
        }
    }

    //BIAS
    for(unsigned layer = 0; layer < m_layers.size() - 1; layer++) {
        auto & cur_neuron = m_layers.at(layer).at(m_layers.at(layer).size() - 1);
        cur_neuron.text_neuron = new QGraphicsTextItem(this);
        neurons->addToGroup(cur_neuron.text_neuron);
        cur_neuron.text_neuron->setPlainText( (bias_preafix ? "Bias 1" : "1"));
        cur_neuron.text_neuron->setPos(cur_neuron.neuronGItem->mapToScene(cur_neuron.neuronGItem->boundingRect().center()) - cur_neuron.neuronGItem->boundingRect().center() - QPointF(neuron_size / 2 + 10 + (bias_preafix ? 20 : 0), 0 ));
    }

    //INIT INPUT TEXT
    unsigned neuron_count =  m_layers.at(0).size() - 1;
    for(unsigned neuron = 0; neuron < neuron_count; neuron++) {
        auto & cur_neuron = m_layers.at(0).at(neuron);
        cur_neuron.text_neuron = new QGraphicsTextItem(this);
        neurons->addToGroup(cur_neuron.text_neuron);
    }

    //INIT OUTPUT TEXT
    neuron_count =  m_layers.back().size() - 1;
    for(unsigned neuron = 0; neuron < neuron_count; neuron++) {
        auto & cur_neuron = m_layers.back().at(neuron);
        cur_neuron.text_neuron = new QGraphicsTextItem(this);
        neurons->addToGroup(cur_neuron.text_neuron);
    }

    updateWeightsLabels();
    updateInputLabels();
    updateOutputLabels();
}

void ViewNet::updateInputLabels(bool color_neuron, int ofset)
{
    if(!net)
        return;
    unsigned neuron_count =  m_layers.at(0).size() - 1;
    for(unsigned neuron = 0; neuron < neuron_count; neuron++) {
        auto & cur_neuron = m_layers.at(0).at(neuron);
        double w = net->getNeuronValue(0, neuron);

        cur_neuron.text_neuron->setPlainText( QString::fromStdString(praefixes.size() > neuron ? praefixes.at(neuron) : "") + QString::number( w ) );
        cur_neuron.text_neuron->setPos(cur_neuron.neuronGItem->mapToScene(cur_neuron.neuronGItem->boundingRect().center()) - cur_neuron.text_neuron->boundingRect().center() - QPointF(neuronSize / 2 + 30 + ofset, 0) );

        if(color_neuron) {
            QColor color;
            if(w > 0)      color =  QColor::fromRgbF(0, 1.0 - std::min( abs(w) , 1.0), 0);
            else if(w < 0) color =  QColor::fromRgbF(1.0 - std::min( abs(w) , 1.0), 0, 0);
            else color = Qt::white;
            cur_neuron.neuronGItem->setBrush(QBrush(color));
        }
    }
}

void ViewNet::updateOutputLabels(bool color_neuron, bool softmax  , int ofset)
{
    if(!net)
        return;
    unsigned neuron_count =  m_layers.back().size() - 1;
    unsigned x = 0;
    double max = 0.0;
    if(softmax) {
        for(unsigned neuron = 0; neuron < neuron_count; neuron++) {
            double w = net->getNeuronValue(m_layers.size() - 1, neuron);
            if(w > max) {
                x = neuron;
                max = w;
            }
        }
    }

    for(unsigned neuron = 0; neuron < neuron_count; neuron++) {
        auto & cur_neuron = m_layers.back().at(neuron);
        double w = net->getNeuronValue(m_layers.size() - 1, neuron);

        cur_neuron.text_neuron->setPlainText( QString::number( w ) + QString::fromStdString(suffixes.size() > neuron ? suffixes.at(neuron) : ""));
        cur_neuron.text_neuron->setPos(cur_neuron.neuronGItem->mapToScene(cur_neuron.neuronGItem->boundingRect().center()) - cur_neuron.text_neuron->boundingRect().center() + QPointF(neuronSize / 2 + 30 + ofset, 0) );
        if(color_neuron) {
            QColor color;
            if(softmax) {
                if(neuron == x)
                    color = Qt::cyan;
                else color = Qt::white;

            } else {
                if(w > 0)      color =  QColor::fromRgbF(0, 1.0 - std::min( abs(w) , 1.0), 0);
                else if(w < 0) color =  QColor::fromRgbF(1.0 - std::min( abs(w) , 1.0), 0, 0);
                else color = Qt::white;
            }

            cur_neuron.neuronGItem->setBrush(QBrush(color));
        }
    }
}

void ViewNet::updateWeightsLabels()
{
    if(!net)
        return;
    for(unsigned layer = 0; layer < m_layers.size() - 1; layer++) { // letzter layer keine connections???????????????????????????????????????????? auch oben schauen, ob so richtig!
        unsigned neuron_count = (m_layers.at(layer).size() - ((layer == m_layers.size() - 1) ? 1 : 0)); // letzter layer keine bios
        for(unsigned neuron = 0; neuron < neuron_count; ++neuron) {
            for(unsigned conns = 0; conns < m_layers.at(layer).at(neuron).m_outputWeights.size(); conns++) {
                auto &con = m_layers.at(layer).at(neuron).m_outputWeights.at(conns);

                double w = net->getConWeight(layer, neuron, conns);
                QColor color;
                if(w > 0)
                    color =  QColor::fromRgbF(0, 1.0 - std::min( abs(w) , 1.0), 0);
                else
                    color =  QColor::fromRgbF(1.0 - std::min( abs(w) , 1.0), 0, 0);

                con.line->setPen(QPen(QBrush(color), con.line->pen().width()));
                con.line->update();
                con.text_weight->setPlainText(QString::number( w ));
            }
        }
    }
}

void ViewNet::changeNet(Net *newNetWithSameTop)
{
    if(!net)
        return;
    net = newNetWithSameTop;
}
