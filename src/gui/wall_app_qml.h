#pragma once
#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace wallchanger::gui {
class application_qml : public QGuiApplication {
  Q_OBJECT
public:
  application_qml(int argc, char *argv[]);
  int run();

private:
  QQmlApplicationEngine m_engine;
};
} // namespace wallchanger::gui