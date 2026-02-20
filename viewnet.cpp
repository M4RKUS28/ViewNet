#include "viewnet.h"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <cmath>

ViewNet::ViewNet(const Net *net, const QRect &rect, int neuron_size,
                 bool show_weights)
    : net(net), size(rect), neuronSize(neuron_size) {
  if (!net)
    return;

  neurons = new QGraphicsItemGroup(this);
  weights = new QGraphicsItemGroup(this);

  this->setRect(rect);
  for (size_t layerNum = 0; layerNum < net->getTopology().size(); ++layerNum) {
    m_layers.push_back(ViewLayer());
    size_t numOutput = (layerNum == net->getTopology().size() - 1)
                           ? 0
                           : net->getTopology().at(layerNum + 1).neuronCount;
    for (size_t neuronNum = 0;
         neuronNum < net->getTopology().at(layerNum).neuronCount + 1;
         ++neuronNum) {
      m_layers.back().push_back(ViewNeuron(numOutput));
    }
  }
  this->resize(rect, neuron_size, 2, show_weights, true);
}

ViewNet::~ViewNet() {
  // Qt memory model automatically manages and deletes child QGraphicsItems
  // when the parent object (this ViewNet) is destroyed.
}

QColor ViewNet::getNeuronColor(double w) const {
  if (w > 0)
    return QColor::fromRgbF(0.0, 1.0 - std::min(std::abs(w), 1.0), 0.0);
  else if (w < 0)
    return QColor::fromRgbF(1.0 - std::min(std::abs(w), 1.0), 0.0, 0.0);
  else
    return Qt::white;
}

void ViewNet::setInputPraefix(const std::vector<std::string> &labels) {
  this->praefixes = labels;
}

void ViewNet::setOutputSuffix(const std::vector<std::string> &labels) {
  this->suffixes = labels;
}

void ViewNet::resize(const QRect &rect, int neuron_size, int line_size,
                     bool show_weights, bool bias_preafix) {
  if (!net)
    return;
  int spacer_x =
      (rect.width() - m_layers.size() * neuronSize) / (m_layers.size() + 1);

  for (size_t layer = 0; layer < m_layers.size(); layer++) {
    size_t neuron_count =
        m_layers.at(layer).size() - ((layer == m_layers.size() - 1) ? 1 : 0);
    int spacer_y =
        (rect.height() - neuron_count * neuronSize) / (neuron_count + 1);

    for (size_t neuron = 0; neuron < neuron_count; neuron++) {
      QPoint neuronPos(neuronSize * layer + spacer_x * (layer + 1),
                       neuronSize * neuron + spacer_y * (neuron + 1));

      auto &curNeuron = m_layers.at(layer).at(neuron);

      // Fix memory leak: Re-use the existing graphic items or create them if
      // they don't exist
      if (!curNeuron.neuronGItem) {
        curNeuron.neuronGItem =
            new QGraphicsEllipseItem(0, 0, neuronSize, neuronSize, this);
        neurons->addToGroup(curNeuron.neuronGItem);
      } else {
        curNeuron.neuronGItem->setRect(0, 0, neuronSize, neuronSize);
      }
      curNeuron.neuronGItem->setPos(neuronPos);

      for (size_t conns = 0; conns < curNeuron.m_outputWeights.size();
           conns++) {
        auto &con = curNeuron.m_outputWeights.at(conns);

        qreal x1 = neuronPos.x() + neuronSize;
        qreal y1 = neuronPos.y() + neuronSize / 2.0;

        size_t next_layer_neurons =
            m_layers.at(layer + 1).size() -
            ((layer + 1 == m_layers.size() - 1) ? 1 : 0);
        qreal x2 = neuronPos.x() + spacer_x + neuronSize;
        qreal y2 = neuronSize * conns +
                   (rect.height() - next_layer_neurons * neuronSize) /
                       (next_layer_neurons + 1) * (conns + 1) +
                   neuronSize / 2.0;

        if (!con.line) {
          con.line = new QGraphicsLineItem(x1, y1, x2, y2, this);
          weights->addToGroup(con.line);
        } else {
          con.line->setLine(x1, y1, x2, y2);
        }
        con.line->setPen(QPen(QBrush(Qt::black), line_size));

        if (!con.text_weight) {
          con.text_weight = new QGraphicsTextItem(
              QString::number(net->getConWeight(layer, neuron, conns)), this);
          weights->addToGroup(con.text_weight);
        } else {
          con.text_weight->setPlainText(
              QString::number(net->getConWeight(layer, neuron, conns)));
        }

        QPointF textPos = con.line->boundingRect().center();
        con.text_weight->setPos(textPos);
        con.text_weight->setVisible(show_weights);
      }
    }
  }

  // BIAS
  for (size_t layer = 0; layer < m_layers.size() - 1; layer++) {
    auto &cur_neuron = m_layers.at(layer).back();
    if (!cur_neuron.text_neuron) {
      cur_neuron.text_neuron = new QGraphicsTextItem(this);
      neurons->addToGroup(cur_neuron.text_neuron);
    }
    cur_neuron.text_neuron->setPlainText((bias_preafix ? "Bias 1" : "1"));
    if (cur_neuron.neuronGItem) {
      QPointF center = cur_neuron.neuronGItem->boundingRect().center();
      QPointF sceneCenter = cur_neuron.neuronGItem->mapToScene(center);
      cur_neuron.text_neuron->setPos(
          sceneCenter - center -
          QPointF(neuronSize / 2.0 + 10.0 + (bias_preafix ? 20.0 : 0.0), 0));
    }
  }

  // INIT INPUT TEXT
  size_t input_neuron_count = m_layers.at(0).size() - 1;
  for (size_t neuron = 0; neuron < input_neuron_count; neuron++) {
    auto &cur_neuron = m_layers.at(0).at(neuron);
    if (!cur_neuron.text_neuron) {
      cur_neuron.text_neuron = new QGraphicsTextItem(this);
      neurons->addToGroup(cur_neuron.text_neuron);
    }
  }

  // INIT OUTPUT TEXT
  size_t output_neuron_count = m_layers.back().size() - 1;
  for (size_t neuron = 0; neuron < output_neuron_count; neuron++) {
    auto &cur_neuron = m_layers.back().at(neuron);
    if (!cur_neuron.text_neuron) {
      cur_neuron.text_neuron = new QGraphicsTextItem(this);
      neurons->addToGroup(cur_neuron.text_neuron);
    }
  }

  updateWeightsLabels();
  updateInputLabels();
  updateOutputLabels();
}

void ViewNet::updateInputLabels(bool color_neuron, int offset) {
  if (!net)
    return;
  size_t neuron_count = m_layers.at(0).size() - 1;
  for (size_t neuron = 0; neuron < neuron_count; neuron++) {
    auto &cur_neuron = m_layers.at(0).at(neuron);
    double w = net->getNeuronValue(0, neuron);

    if (cur_neuron.text_neuron) {
      cur_neuron.text_neuron->setPlainText(
          QString::fromStdString(
              praefixes.size() > neuron ? praefixes.at(neuron) : "") +
          QString::number(w));

      if (cur_neuron.neuronGItem) {
        QPointF center = cur_neuron.neuronGItem->boundingRect().center();
        QPointF sceneCenter = cur_neuron.neuronGItem->mapToScene(center);
        cur_neuron.text_neuron->setPos(
            sceneCenter - cur_neuron.text_neuron->boundingRect().center() -
            QPointF(neuronSize / 2.0 + 30.0 + offset, 0));
      }
    }

    if (color_neuron && cur_neuron.neuronGItem) {
      cur_neuron.neuronGItem->setBrush(QBrush(getNeuronColor(w)));
    }
  }
}

void ViewNet::updateOutputLabels(bool color_neuron, bool softmax, int offset) {
  if (!net)
    return;
  size_t neuron_count = m_layers.back().size() - 1;
  size_t max_idx = 0;
  double max_val = 0.0;

  if (softmax && neuron_count > 0) {
    max_val = net->getNeuronValue(m_layers.size() - 1, 0);
    for (size_t neuron = 0; neuron < neuron_count; neuron++) {
      double w = net->getNeuronValue(m_layers.size() - 1, neuron);
      if (w > max_val) {
        max_idx = neuron;
        max_val = w;
      }
    }
  }

  for (size_t neuron = 0; neuron < neuron_count; neuron++) {
    auto &cur_neuron = m_layers.back().at(neuron);
    double w = net->getNeuronValue(m_layers.size() - 1, neuron);

    if (cur_neuron.text_neuron) {
      cur_neuron.text_neuron->setPlainText(
          QString::number(w) + QString::fromStdString(suffixes.size() > neuron
                                                          ? suffixes.at(neuron)
                                                          : ""));

      if (cur_neuron.neuronGItem) {
        QPointF center = cur_neuron.neuronGItem->boundingRect().center();
        QPointF sceneCenter = cur_neuron.neuronGItem->mapToScene(center);
        cur_neuron.text_neuron->setPos(
            sceneCenter - cur_neuron.text_neuron->boundingRect().center() +
            QPointF(neuronSize / 2.0 + 30.0 + offset, 0));
      }
    }

    if (color_neuron && cur_neuron.neuronGItem) {
      if (softmax) {
        cur_neuron.neuronGItem->setBrush(
            QBrush(neuron == max_idx ? Qt::cyan : Qt::white));
      } else {
        cur_neuron.neuronGItem->setBrush(QBrush(getNeuronColor(w)));
      }
    }
  }
}

void ViewNet::updateWeightsLabels() {
  if (!net)
    return;
  for (size_t layer = 0; layer < m_layers.size() - 1; layer++) {
    size_t neuron_count =
        m_layers.at(layer).size() - ((layer == m_layers.size() - 1) ? 1 : 0);
    for (size_t neuron = 0; neuron < neuron_count; ++neuron) {
      for (size_t conns = 0;
           conns < m_layers.at(layer).at(neuron).m_outputWeights.size();
           conns++) {
        auto &con = m_layers.at(layer).at(neuron).m_outputWeights.at(conns);

        double w = net->getConWeight(layer, neuron, conns);
        QColor color = getNeuronColor(w);

        if (con.line) {
          con.line->setPen(QPen(QBrush(color), con.line->pen().width()));
          con.line->update();
        }
        if (con.text_weight) {
          con.text_weight->setPlainText(QString::number(w));
        }
      }
    }
  }
}

void ViewNet::changeNet(const Net *newNetWithSameTop) {
  if (!net)
    return;
  net = newNetWithSameTop;
}
