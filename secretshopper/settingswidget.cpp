#include "settingswidget.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

SettingsWidget::SettingsWidget(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);

    auto* grp = new QGroupBox("⚙️ Настройки оптимизации");
    auto* gl  = new QVBoxLayout(grp);

    m_loyalty = new QCheckBox("Учитывать программы лояльности (карты, кэшбэк)");
    m_loyalty->setChecked(true);
    m_loyalty->setStyleSheet("font-size:13px;");

    m_route = new QCheckBox("Оптимизировать маршрут (алгоритм NN-TSP + 2-opt)");
    m_route->setChecked(true);
    m_route->setStyleSheet("font-size:13px;");

    auto* info = new QLabel(
        "<span style='color:#7f8c8d;font-size:11px;'>"
        "Алгоритм TSP (Nearest Neighbor + 2-opt) минимизирует расстояние обхода магазинов. "
        "Лояльность учитывает скидки по картам и кэшбэк."
        "</span>");
    info->setWordWrap(true);

    gl->addWidget(m_loyalty);
    gl->addWidget(m_route);
    gl->addWidget(info);
    root->addWidget(grp);
}

bool SettingsWidget::useLoyalty()    const { return m_loyalty->isChecked(); }
bool SettingsWidget::optimizeRoute() const { return m_route->isChecked(); }
