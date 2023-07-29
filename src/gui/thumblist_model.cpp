#include "thumblist_model.h"

ThumbListModel::ThumbListModel(QObject *parent) : QAbstractListModel{parent} {}

int ThumbListModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return 0;
  }
  return m_data.count();
}

QVariant ThumbListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return {};
  }

  const auto &data = m_data.get_data_at(index.row());
  if (role == Qt::DisplayRole) {
    return data.first;
  }
  if (role == Qt::DecorationRole) {
    return data.second;
  }

  return {};
}

void ThumbListModel::set_model_data(ThumbListData new_data)
{
  m_data = new_data;
}
