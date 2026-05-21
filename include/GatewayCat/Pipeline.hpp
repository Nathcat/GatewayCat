#ifndef GATEWAYCAT_PIPELINE
#define GATEWAYCAT_PIPELINE

#include <cstring>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace nathcat {
namespace gateway {
namespace pipeline {

template <typename T> class Pipeline;

/**
 * @class Layer
 * @brief A processing layer. Accepts data and processes it, then passes down
 * the pipeline. All methods which operate on the internal data queue are thread
 * safe and protected via mutexes.
 *
 */
template <typename T> class Layer {
private:
  Pipeline<T> *parent;
  int id;
  std::queue<T> queue;
  std::mutex queueMutex;

public:
  Layer<T>() {}
  Layer<T>(Layer &l) {
    this->parent = l.parent;
    this->id = l.id;
    this->queue = l.queue;
  }

  void init(Pipeline<T> *parent, const int id) {
    this->parent = parent;
    this->id = id;
  }

  Pipeline<T> *getParent() { return this->parent; }
  int getId() { return this->id; }

  /**
   * @brief Intake a data object to be processed into the queue
   *
   * @param d The data object to accept
   */
  void operator<<(T d) {
    queueMutex.lock();
    queue.push(d);
    queueMutex.unlock();
  }

  /**
   * @brief Send a data object to the next layer
   *
   * @param d The data object to send
   */
  void operator>>(T d) {
    Layer<T> *next = parent->getLayer(id + 1);
    (*next) << d;
  }

  /**
   * @brief Get the next item in the queue. Will block until a new object can be
   * obtained. If the pipeline stops running while waiting, this function will
   * immediately return NULL.
   *
   * @paramn out Output for the next object
   * @return true if a new object was returned, false otherwise
   */
  bool getNext(T *out) {
    int queueSize = 0;

    while (queueSize == 0) {
      queueMutex.lock();
      queueSize = queue.size();
      queueMutex.unlock();

      if (!getParent()->running())
        return false;
    }

    queueMutex.lock();
    T d = queue.front();
    queue.pop();
    queueMutex.unlock();

    std::memcpy(out, &d, sizeof(T));

    return true;
  }

  /**
   * @brief Thread execution body. Data acceptance and processing should be done
   * here
   */
  virtual void operator()() = 0;
};

/**
 * @brief Start a new thread for a layer
 *
 * @tparam T The type accepted by the layer
 * @param layer The layer to start
 * @return The new thread in which the layer is running
 */
template <typename T> std::thread start_layer(Layer<T> &layer) {
  std::thread t([](Layer<T> &layer) { layer(); }, std::ref(layer));

  return t;
}

/**
 * @class Pipeline
 * @brief A pipeline allows the passing of data through some handling stream.
 * Layers are used as processing points, which accept data and process it. The
 * data is sourced, and then queued into the pipeline, ready to be accepted by
 * the first layer. The first layer accepts from this queue, processes the data,
 * and queues it into the next layer, until the end of the pipeline is reached.
 *
 */
template <typename T> class Pipeline {
private:
  std::vector<Layer<T> *> layers;
  bool *runningState;
  std::vector<std::vector<std::thread>> threads;

public:
  /**
   * @brief Insert a data object into the pipeline
   *
   * @param d The data object to insert
   */
  void operator<<(T d) { (*(layers.front())) << d; }

  Layer<T> *getLayer(int id) { return layers[id]; }

  void appendLayer(Layer<T> *layer) {
    layer->init(this, layers.size());
    layers.push_back(layer);
  }

  bool running() { return *runningState; }

  Pipeline(bool *runningState) { this->runningState = runningState; }

  Pipeline(bool *runningState, std::vector<Layer<T> *> layers) {
    this->runningState = runningState;
    this->layers = layers;
  }

  Pipeline(Pipeline &p) {
    this->runningState = p.runningState;
    this->layers = p.layers;
  }

  Pipeline &operator=(Pipeline &p) {
    this->runningState = p.runningState;
    this->layers = p.layers;

    return *this;
  }

  ~Pipeline() {
    for (int i = 0; i < layers.size(); i++) {
      free(layers.at(i));
    }
  }

  /**
   * @brief Start the pipeline
   *
   * @param threadCount The number of threads to start per layer
   */
  void start(int threadCount) {
    for (int i = 0; i < layers.size(); i++) {
      threads.push_back(std::vector<std::thread>());

      for (int j = 0; j < threadCount; j++) {
        threads.at(i).push_back(start_layer(*(layers.at(i))));
      }
    }
  }

  /**
   * @brief Waits for all threads to join the main thread.
   */
  void waitForStop() {
    for (int i = 0; i < threads.size(); i++) {
      for (int j = 0; j < threads.at(i).size(); j++) {
        threads.at(i).at(j).join();
      }
    }
  }
};
}; // namespace pipeline
}; // namespace gateway
}; // namespace nathcat

#endif
