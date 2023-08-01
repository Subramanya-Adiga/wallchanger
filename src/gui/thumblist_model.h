#pragma once

#include "thumblist_data.h"
#include <QAbstractListModel>

namespace wallchanger::gui {

static constexpr int IMG_MEMORY_ALLOC = 1024;
static constexpr int IMG_RES = 256;

class ThumbListModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit ThumbListModel(const QString &path, QObject *parent = nullptr);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index,
                              int role) const override;
  [[nodiscard]] const ThumbListData *get_model_data() const;

signals:
  void thumb_nail_ready(QString, QImage);
  void directory_processed();

private:
  QSharedPointer<ThumbListData> m_data;
  int m_counter{0};
  void m_add_data(QString name, QImage image);
  void m_set_directory(const std::filesystem::path &path);
};
} // namespace wallchanger::gui
