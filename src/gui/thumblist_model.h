#pragma once

#include <QAbstractListModel>
#include "thumblist_data.h"

class ThumbListModel : public QAbstractListModel {
public:
  explicit ThumbListModel(QObject *parent = nullptr);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

  void set_model_data(ThumbListData new_data);
private:
  ThumbListData m_data;
};
