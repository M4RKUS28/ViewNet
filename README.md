# ViewNet

A Qt-based C++ library for visualizing neural network architectures and real-time network states.

## Overview

ViewNet is a static library that provides graphical visualization capabilities for neural networks. It renders neural network topologies as interactive graphics, showing neurons, connections, weights, and activation values in real-time. The library is designed to work with the GenNet neural network library.

## Features

- **Network Visualization**: Render complete neural network architectures with neurons and connections
- **Real-time Updates**: Dynamic updating of neuron values, weights, and activations
- **Customizable Display**: Configurable neuron sizes, line widths, and weight visibility
- **Color Coding**: Visual representation of activation values and weight strengths
- **Label Support**: Custom labels for input and output neurons
- **Bias Neurons**: Special handling and visualization of bias neurons

## Dependencies

- **Qt Framework**: Core, GUI, and Widgets modules (Qt 5 or Qt 6)
- **GenNet Library**: Neural network implementation (external dependency)
- **C++17**: Modern C++ standard support

## Build Requirements

- Qt 5.x or Qt 6.x
- C++17 compliant compiler
- qmake build system
- GenNet library (linked from `../GenNet` directory)

## Installation

1. Ensure Qt and the GenNet library are properly installed
2. Build using qmake:
   ```bash
   qmake ViewNet.pro
   make
   ```

## Usage

### Basic Usage

```cpp
#include "viewnet.h"
#include "net.h"

// Create a neural network
Net* network = new Net(topology);

// Create visualization
QRect viewRect(0, 0, 800, 600);
ViewNet* viewNet = new ViewNet(network, viewRect, 40, true);

// Add to graphics scene
QGraphicsScene* scene = new QGraphicsScene();
scene->addItem(viewNet);
```

### Key Methods

- `ViewNet(Net* net, QRect rect, int neuron_size, bool show_weights)`: Constructor
- `resize(QRect size, int pixelSize, int line_size, bool show_weights, bool bias_prefix)`: Resize and reconfigure display
- `updateInputLabels(bool color_neuron, int offset)`: Update input neuron labels and colors
- `updateOutputLabels(bool color_neuron, bool softmax, int offset)`: Update output neuron labels and colors
- `updateWeightsLabels()`: Update weight visualizations and colors
- `setInputPraefix(const std::vector<std::string>& labels)`: Set custom input labels
- `setOutputSuffix(const std::vector<std::string>& labels)`: Set custom output labels

### Color Coding

- **Positive Values**: Green gradient (darker = stronger)
- **Negative Values**: Red gradient (darker = stronger)
- **Zero Values**: White
- **Softmax Max**: Cyan (for output layer when using softmax)

## Architecture

### Core Classes

- **ViewNet**: Main visualization class inheriting from `QGraphicsRectItem`
- **ViewNeuron**: Represents individual neurons with connections
- **ViewConnection**: Handles connection lines and weight labels

### Data Structures

- `ViewLayer`: Vector of neurons representing a network layer
- `ViewConnection`: Contains line item and weight text for connections
- Graphics items organized in groups for efficient rendering

## Integration

ViewNet is designed as a static library and can be integrated into Qt applications that require neural network visualization. The library depends on the GenNet library for neural network data structures and operations.