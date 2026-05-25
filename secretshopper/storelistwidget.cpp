#include "storelistwidget.h"
#include "mockpriceserver.h"
#include "loyaltysystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QColor>         // FIX: добавлен include для QColor
#include <QItemSelectionModel>
#include <QSignalBlocker>
#include <algorithm>

// Колонки таблицы
enum Col { C_ID=0, C_NAME, C_CHAIN, C_ADDRESS, C_X, C_Y,
           C_SOURCE, C_ENDPOINT, C_LOYALTY, C_COUNT };

static const QStringList HEADERS = {
    "ID","Название","Сеть","Адрес","X","Y","Источник","URL/API","Лояльность"
};

StoreListWidget::StoreListWidget(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);

    auto* grp = new QGroupBox("🏪 Список магазинов");
    auto* gl  = new QVBoxLayout(grp);

    auto* btnRow = new QHBoxLayout();
    auto* addBtn = new QPushButton("＋ Добавить магазин");
    addBtn->setStyleSheet(
        "QPushButton{background:#27ae60;color:white;border-radius:4px;padding:6px 14px;font-weight:bold;}"
        "QPushButton:hover{background:#1e8449;}");
    connect(addBtn, &QPushButton::clicked, this, &StoreListWidget::addRow);

    auto* delBtn = new QPushButton("✕ Удалить");
    delBtn->setStyleSheet(
        "QPushButton{background:#e74c3c;color:white;border-radius:4px;padding:6px 14px;}"
        "QPushButton:hover{background:#c0392b;}");
    connect(delBtn, &QPushButton::clicked, this, &StoreListWidget::removeSelected);

    auto* helpLbl = new QLabel(
        "<i>Источник «Встроенный» — данные уже в программе. "
        "«Сеть (URL)» — реальный HTTP-запрос к API/сайту.</i>");
    helpLbl->setStyleSheet("color:#7f8c8d;font-size:11px;");
    helpLbl->setWordWrap(true);

    btnRow->addWidget(addBtn);
    btnRow->addWidget(delBtn);
    btnRow->addStretch();
    gl->addLayout(btnRow);
    gl->addWidget(helpLbl);

    m_table = new QTableWidget(0, C_COUNT);
    m_table->setHorizontalHeaderLabels(HEADERS);
    m_table->horizontalHeader()->setSectionResizeMode(C_NAME,     QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(C_ADDRESS,  QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(C_ENDPOINT, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(C_ID,       QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(C_X,        QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(C_Y,        QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(C_LOYALTY,  QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget{border:1px solid #bbb;border-radius:4px;}"
        "QHeaderView::section{background:#2c3e50;color:white;padding:5px;font-weight:bold;}");

    connect(m_table, &QTableWidget::cellChanged, this, &StoreListWidget::onCellChanged);
    gl->addWidget(m_table, 1);
    root->addWidget(grp);
}

void StoreListWidget::insertStoreRow(int row, const Store& s) {
    QSignalBlocker blocker(m_table);
    m_table->insertRow(row);

    auto mkItem = [](const QString& t, bool editable = true) {
        auto* i = new QTableWidgetItem(t);
        if (!editable) i->setFlags(i->flags() & ~Qt::ItemIsEditable);
        return i;
    };

    m_table->setItem(row, C_ID,      mkItem(QString::number(s.id), false));
    m_table->setItem(row, C_NAME,    mkItem(s.name));
    m_table->setItem(row, C_ADDRESS, mkItem(s.address));
    m_table->setItem(row, C_X,       mkItem(QString::number(s.coords.x(), 'f', 1)));
    m_table->setItem(row, C_Y,       mkItem(QString::number(s.coords.y(), 'f', 1)));
    m_table->setItem(row, C_ENDPOINT,mkItem(s.apiEndpoint));

    auto* chainBox = new QComboBox();
    chainBox->addItem("— не сетевой —");
    for (const QString& ch : MockPriceServer::supportedChains())
        chainBox->addItem(ch);
    int idx = chainBox->findText(s.chain);
    if (idx >= 0) chainBox->setCurrentIndex(idx);

    connect(chainBox, &QComboBox::currentTextChanged,
            this, [this, chainBox](const QString& /*ch*/) {
        const QModelIndex index = m_table->indexAt(chainBox->pos());
        if (index.isValid()) {
            onCellChanged(index.row(), C_CHAIN);
        }
        emit storesChanged();
    });
    m_table->setCellWidget(row, C_CHAIN, chainBox);

    auto* srcBox = new QComboBox();
    srcBox->addItem("Встроенный (Mock)");
    srcBox->addItem("Сеть (URL)");
    srcBox->setCurrentIndex(s.priceSource == PriceSource::Network ? 1 : 0);
    connect(srcBox, &QComboBox::currentTextChanged, this, &StoreListWidget::storesChanged);
    m_table->setCellWidget(row, C_SOURCE, srcBox);

    auto* loyaltyItem = new QTableWidgetItem(
        s.loyalty.active ? s.loyalty.name : "—");
    loyaltyItem->setFlags(loyaltyItem->flags() & ~Qt::ItemIsEditable);
    loyaltyItem->setForeground(s.loyalty.active ? QColor("#27ae60") : QColor("#7f8c8d"));
    m_table->setItem(row, C_LOYALTY, loyaltyItem);

}

void StoreListWidget::addRow() {
    Store s;
    s.id      = m_nextId++;
    s.name    = QString("Магазин %1").arg(s.id);
    s.address = "ул. Примерная, 1";
    s.coords  = MockPriceServer::defaultCoords(s.id);
    insertStoreRow(m_table->rowCount(), s);
    emit storesChanged();
}

void StoreListWidget::removeSelected() {
    QList<int> rows;
    for (const QModelIndex& index : m_table->selectionModel()->selectedRows()) {
        rows.append(index.row());
    }
    std::sort(rows.rbegin(), rows.rend());
    for (int r : rows) m_table->removeRow(r);
    if (!rows.isEmpty()) {
        emit storesChanged();
    }
}

void StoreListWidget::onCellChanged(int row, int col) {
    if (col != C_CHAIN) return;
    auto* chainBox = qobject_cast<QComboBox*>(m_table->cellWidget(row, C_CHAIN));
    if (!chainBox) return;
    QString chain = chainBox->currentText();
    LoyaltyProgram lp = LoyaltySystem::detect(chain);
    auto* li = m_table->item(row, C_LOYALTY);
    if (li) {
        li->setText(lp.active ? lp.name : "—");
        li->setForeground(lp.active ? QColor("#27ae60") : QColor("#7f8c8d"));
    }
}

void StoreListWidget::loadDefaults() {
    m_table->setRowCount(0);
    m_nextId = 1;

    struct Demo { QString name, chain, address; QPointF c; };
    QVector<Demo> demos = {
        {"Пятёрочка на Ленина", "Пятёрочка", "ул. Ленина, 12",  {1.0, 2.0}},
        {"Магнит у площади",    "Магнит",     "пр. Мира, 55",    {4.5, 1.0}},
        {"ВкусВилл Садовая",    "ВкусВилл",   "ул. Садовая, 3",  {3.0, 5.0}},
    };

    for (const Demo& d : demos) {
        Store s;
        s.id          = m_nextId++;
        s.name        = d.name;
        s.chain       = d.chain;
        s.address     = d.address;
        s.coords      = d.c;
        s.loyalty     = LoyaltySystem::detect(d.chain);
        s.priceSource = PriceSource::Mock;
        insertStoreRow(m_table->rowCount(), s);
    }
    emit storesChanged();
}

QVector<Store> StoreListWidget::stores() const {
    QVector<Store> result;
    for (int row = 0; row < m_table->rowCount(); ++row) {
        Store s;
        s.id      = m_table->item(row, C_ID)      ? m_table->item(row, C_ID)->text().toInt()      : 0;
        s.name    = m_table->item(row, C_NAME)     ? m_table->item(row, C_NAME)->text()            : "";
        s.address = m_table->item(row, C_ADDRESS)  ? m_table->item(row, C_ADDRESS)->text()         : "";
        double x  = m_table->item(row, C_X)        ? m_table->item(row, C_X)->text().toDouble()    : 0.0;
        double y  = m_table->item(row, C_Y)        ? m_table->item(row, C_Y)->text().toDouble()    : 0.0;
        s.coords  = QPointF(x, y);
        s.apiEndpoint = m_table->item(row, C_ENDPOINT)
                            ? m_table->item(row, C_ENDPOINT)->text() : "";

        auto* cb  = qobject_cast<QComboBox*>(m_table->cellWidget(row, C_CHAIN));
        s.chain   = cb ? cb->currentText() : "";

        auto* sb  = qobject_cast<QComboBox*>(m_table->cellWidget(row, C_SOURCE));
        s.priceSource = (sb && sb->currentIndex() == 1)
                            ? PriceSource::Network : PriceSource::Mock;

        s.loyalty = LoyaltySystem::detect(s.chain);
        result.append(s);
    }
    return result;
}
