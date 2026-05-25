#pragma once
#include <QWidget>
#include <QCheckBox>
#include <QLabel>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    bool useLoyalty()    const;
    bool optimizeRoute() const;
private:
    QCheckBox* m_loyalty;
    QCheckBox* m_route;
};
