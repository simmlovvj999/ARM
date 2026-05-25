#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QVector>
#include "models.h"

class StoreListWidget : public QWidget {
    Q_OBJECT
public:
    explicit StoreListWidget(QWidget* parent = nullptr);

    QVector<Store> stores() const;
    void loadDefaults();

signals:
    void storesChanged();

private slots:
    void addRow();
    void removeSelected();
    void onCellChanged(int row, int col);

private:
    void insertStoreRow(int row, const Store& s);

    QTableWidget* m_table;
    int m_nextId = 1;
};
