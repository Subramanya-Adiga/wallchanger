#include "wall_app_qml.h"
#include "thumblist_model.h"
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>

wallchanger::gui::application_qml::application_qml(int argc, char *argv[])
    : QGuiApplication(argc, argv) {
  QGuiApplication::setApplicationDisplayName("change_gui_qml");
  QGuiApplication::setApplicationName("changer_gui_qml");
  auto *model = new wallchanger::gui::ThumbListModel("D:/wallpaper", this);
  m_engine.rootContext()->setContextProperty("_mymodel", model);
}

int wallchanger::gui::application_qml::run() {
  auto path = QUrl("qrc:/Main.qml");
  QObject::connect(&m_engine, &QQmlApplicationEngine::objectCreationFailed,
                   [](const QUrl &data) {
//                     QMessageBox::critical(
//                         nullptr, QGuiApplication::applicationDisplayName(),
//                         data.fileName());
                     QCoreApplication::exit(-1);
                   });
  m_engine.load(path);

  return QGuiApplication::exec();
}
