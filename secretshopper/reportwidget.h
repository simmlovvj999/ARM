#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include "models.h"

class ReportWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReportWidget(QWidget* parent = nullptr);

    void showLoading(int pct, const QString& msg = {});
    void showRoute(const ShoppingRoute& route, const QVector<Store>& stores);
    void showError(const QString& msg);
    void clear();

private:
    QString buildHtml(const ShoppingRoute& route, const QVector<Store>& stores);

    QTextEdit*    m_text;
    QLabel*       m_summary;
    QProgressBar* m_progress;
    QPushButton*  m_copyBtn;
};
