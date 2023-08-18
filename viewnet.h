#ifndef VIEWNET_H
#define VIEWNET_H


#include <QGraphicsRectItem>
#include "net.h"
#include <vector>
#include <QGraphicsEllipseItem>


struct ViewNeuron
{
    struct ViewConnection
    {
        ViewConnection() : line(nullptr), text_weight(nullptr) {}
        ViewConnection(QGraphicsLineItem *line, QGraphicsTextItem*text_weight) : line(line), text_weight(text_weight) {}
        QGraphicsLineItem * line;
        QGraphicsTextItem * text_weight;
    };

    ViewNeuron(unsigned connections_count)
        : connections_count(connections_count)
    {
        for (unsigned c = 0; c < connections_count; ++c)  {
            m_outputWeights.push_back(ViewConnection());
        }

    }

    std::vector<ViewConnection> m_outputWeights;
    QGraphicsEllipseItem * neuronGItem;
    QGraphicsTextItem * text_neuron;


    int connections_count;

};


typedef std::vector<ViewNeuron> ViewLayer;


class ViewNet : public QGraphicsRectItem
{
public:
    ViewNet(Net *net, QRect size, int neuronSize = 40, bool show_weights = true);
    ~ViewNet();

    void setInputPraefix(const std::vector<std::string> &labels);
    void setOutputSuffix(const std::vector<std::string> &labels);

    void resize(QRect size, int pixelSize, int line_size, bool show_weights, bool bias_preafix = false);

    void updateInputLabels(bool color_neuron = false, int ofset = 0);
    void updateOutputLabels(bool color_neuron = false, bool softmax = false, int ofset = 0);
    void updateWeightsLabels();

    void changeNet(Net * newNetWithSameTop);

private:
    Net * net;
    QRect size;
    int neuronSize;

    QGraphicsItemGroup * neurons;
    QGraphicsItemGroup * weights;
    std::vector<std::string> suffixes;
    std::vector<std::string> praefixes;
    std::vector<ViewLayer> m_layers;


};

#endif // VIEWNET_H
