#include <GatewayCat/Pipeline.hpp>
using namespace nathcat::gateway;

template <typename T>
void nathcat::gateway::pipeline::Layer<T>::operator<<(T d) {
  queueMutex.lock();
  queue.push(d);
  queueMutex.unlock();
}

template <typename T>
void nathcat::gateway::pipeline::Layer<T>::operator>>(T d) {
  Layer<T> *next = parent->getLayer(id + 1);
  (*next) << d;
}

template <typename T> T nathcat::gateway::pipeline::Layer<T>::getNext() {
  int queueSize = 0;

  while (queueSize == 0) {
    queueMutex.lock();
    queueSize = queue.size();
    queueMutex.unlock();

    if (!getParent()->running())
      return NULL;
  }

  queueMutex.lock();
  T d = queue.front();
  queue.pop();
  queueMutex.unlock();

  return d;
}

template <typename T>
std::thread nathcat::gateway::pipeline::start_layer(Layer<T> &layer) {
  std::thread t([](Layer<T> &layer) { layer(); }, std::ref(layer));

  return t;
}

template <typename T>
void nathcat::gateway::pipeline::Pipeline<T>::start(int threadCount) {
  for (int i = 0; i < layers.size(); i++) {
    threads.push_back(std::vector<std::thread>());

    for (int j = 0; j < threadCount; j++) {
      threads.at(i).push_back(start_layer(layers.at(i)));
    }
  }
}

template <typename T>
void nathcat::gateway::pipeline::Pipeline<T>::waitForStop() {
  for (int i = 0; i < threads.size(); i++) {
    for (int j = 0; j < threads.at(i).size(); j++) {
      threads.at(i).at(j).join();
    }
  }
}
