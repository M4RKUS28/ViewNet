#ifndef VIEWNET_H
#define VIEWNET_H

#include <QGraphicsRectItem>
#include <QStringList>
#include <vector>

#include "net.h"

// Forward declarations to reduce compilation dependencies
class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsTextItem;
class QGraphicsItemGroup;

/**
 * @brief Represents a single visual connection (weight) between two neurons.
 */
struct ViewNeuron {
  struct ViewConnection {
    ViewConnection() = default;
    ViewConnection(QGraphicsLineItem *line, QGraphicsTextItem *text_weight)
        : line(line), text_weight(text_weight) {}

    QGraphicsLineItem *line = nullptr;
    QGraphicsTextItem *text_weight = nullptr;
  };

  explicit ViewNeuron(size_t connections_count) {
    m_outputWeights.resize(connections_count);
  }

  std::vector<ViewConnection> m_outputWeights;
  QGraphicsEllipseItem *neuronGItem = nullptr;
  QGraphicsTextItem *text_neuron = nullptr;
};

using ViewLayer = std::vector<ViewNeuron>;

/**
 * @brief A QGraphicsItem that visualizes a neural network model.
 *
 * Draws the layers, neurons, and connections. It only reads the topology
 * and weights from the `Net` instance.
 */
class ViewNet : public QGraphicsRectItem {
public:
  /**
   * @brief Constructor for the network view.
   * @param net Pointer to the neural network model (non-owning).
   * @param size Bounding rectangle for the drawing.
   * @param neuronSize Diameter of drawn neurons.
   * @param show_weights Whether to display numerical weights on connections.
   */
  ViewNet(const Net *net, const QRect &size, int neuronSize = 40,
          bool show_weights = true);
  ~ViewNet() override;

  void setInputPrefix(const QStringList &labels);
  void setOutputSuffix(const QStringList &labels);

  /**
   * @brief Resizes and positions all graphic items for the network.
   * Allocates items if they don't exist, updates their properties otherwise to
   * avoid memory leaks.
   */
  void resize(const QRect &size, int pixelSize, int line_size,
              bool show_weights, bool bias_prefix = false);

  void updateInputLabels(bool color_neuron = false, int offset = 0);
  void updateOutputLabels(bool color_neuron = false, bool softmax = false,
                          int offset = 0);
  void updateWeightsLabels();

  /**
   * @brief Swaps the observed network. The new net must have the same topology.
   * @param newNet Non-null pointer to a Net with identical topology.
   */
  void changeNet(const Net *newNet);

private:
  // --- Layout constants ---
  static constexpr double kLabelMargin = 30.0;
  static constexpr double kBiasLabelMargin = 10.0;
  static constexpr double kBiasLabelExtraOffset = 20.0;
  static constexpr int kDefaultLineWidth = 2;

  // --- Helper methods (called from resize) ---
  void layoutNeurons(const QRect &rect, int neuron_size, int line_size,
                     bool show_weights);
  void layoutBiasLabels(bool bias_prefix);
  void initInputTextItems();
  void initOutputTextItems();

  QColor getNeuronColor(double w) const;

  const Net *net = nullptr;
  QRect size;
  int neuronSize = 40;

  QGraphicsItemGroup *neurons = nullptr;
  QGraphicsItemGroup *weights = nullptr;
  QStringList suffixes;
  QStringList prefixes;
  std::vector<ViewLayer> m_layers;
};

#endif // VIEWNET_H
