#include "reportwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>

ReportWidget::ReportWidget(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(6);

    m_summary = new QLabel("Здесь появится оптимальный маршрут покупок.");
    m_summary->setWordWrap(true);
    m_summary->setStyleSheet(
        "QLabel{background:#2c3e50;color:white;padding:10px 14px;"
        "border-radius:6px;font-size:13px;font-weight:bold;}");
    root->addWidget(m_summary);

    m_progress = new QProgressBar();
    m_progress->setRange(0, 100);
    m_progress->setVisible(false);
    m_progress->setStyleSheet(
        "QProgressBar{border:1px solid #bbb;border-radius:4px;height:14px;}"
        "QProgressBar::chunk{background:#3498db;border-radius:4px;}");
    root->addWidget(m_progress);

    m_text = new QTextEdit();
    m_text->setReadOnly(true);
    m_text->setMinimumHeight(380);
    m_text->setStyleSheet(
        "QTextEdit{border:1px solid #ddd;border-radius:6px;background:white;"
        "font-family:'Segoe UI',Arial,sans-serif;font-size:12px;}");
    root->addWidget(m_text, 1);

    auto* btnRow = new QHBoxLayout();
    m_copyBtn = new QPushButton("📋 Скопировать как текст");
    m_copyBtn->setStyleSheet(
        "QPushButton{background:#7f8c8d;color:white;border-radius:4px;padding:7px 16px;}"
        "QPushButton:hover{background:#636e72;}");
    connect(m_copyBtn, &QPushButton::clicked, this, [this]{
        QApplication::clipboard()->setText(m_text->toPlainText());
    });
    btnRow->addStretch();
    btnRow->addWidget(m_copyBtn);
    root->addLayout(btnRow);
}

void ReportWidget::clear() {
    m_text->clear();
    m_progress->setVisible(false);
    m_summary->setText("Здесь появится оптимальный маршрут покупок.");
}

void ReportWidget::showLoading(int pct, const QString& msg) {
    m_progress->setVisible(true);
    m_progress->setValue(pct);
    m_summary->setText(msg.isEmpty()
        ? QString("⏳ Загружаю цены... %1%").arg(pct)
        : msg);
}

void ReportWidget::showError(const QString& msg) {
    m_progress->setVisible(false);
    m_summary->setText("❌ " + msg);
    m_text->setHtml(QString("<p style='color:red;'>%1</p>").arg(msg.toHtmlEscaped()));
}

void ReportWidget::showRoute(const ShoppingRoute& route, const QVector<Store>& stores) {
    m_progress->setVisible(false);

    if (route.isEmpty()) {
        m_summary->setText("⚠️ Не найдено ни одного товара в указанных магазинах.");
        m_text->clear();
        return;
    }

    QString savPct = route.totalRegular > 0
        ? QString::number(route.totalSavings / route.totalRegular * 100.0, 'f', 1)
        : "0";

    m_summary->setText(
        QString("💰 Без скидок: %1 ₽  |  💳 С лояльностью: %2 ₽  |  "
                "🎉 Экономия: %3 ₽ (%4%)  |  🗺 Маршрут: ~%5 усл.ед.")
            .arg(route.totalRegular, 0,'f',2)
            .arg(route.totalFinal,   0,'f',2)
            .arg(route.totalSavings, 0,'f',2)
            .arg(savPct)
            .arg(route.totalDistance,0,'f',1));

    m_text->setHtml(buildHtml(route, stores));
}

// ─────────────────────────────────────────────────────────
QString ReportWidget::buildHtml(const ShoppingRoute& route,
                                 const QVector<Store>& stores)
{
    auto findStore = [&](int id) -> const Store* {
        for (const Store& s : stores) if (s.id == id) return &s;
        return nullptr;
    };

    QString h;
    h += R"(
<style>
  body{font-family:'Segoe UI',Arial,sans-serif;color:#2c3e50;margin:0;padding:0;}
  h2{color:#2c3e50;border-bottom:2px solid #3498db;padding-bottom:5px;margin-top:10px;}
  h3{color:#2980b9;margin-top:14px;margin-bottom:4px;}
  table{border-collapse:collapse;width:100%;margin-bottom:10px;font-size:12px;}
  th{background:#2c3e50;color:white;padding:7px 10px;text-align:left;}
  td{padding:6px 10px;border-bottom:1px solid #ecf0f1;}
  tr:hover td{background:#f5f9fc;}
  .block{background:#f4f8fb;border-left:4px solid #3498db;
         padding:8px 12px;margin:10px 0;border-radius:3px;}
  .badge{background:#e8f5e9;color:#2e7d32;border-radius:10px;
         padding:1px 7px;font-size:11px;margin-left:6px;}
  .warn{background:#fff3e0;border-left:4px solid #ff9800;
        padding:8px 12px;margin:10px 0;border-radius:3px;}
  .tot td{background:#2c3e50;color:white;font-weight:bold;}
  .green{color:#27ae60;font-weight:bold;}
  .step{display:inline-block;background:#3498db;color:white;border-radius:50%;
        width:22px;height:22px;text-align:center;line-height:22px;
        font-weight:bold;font-size:12px;margin-right:5px;}
  .src{font-size:10px;color:#95a5a6;margin-left:8px;}
</style>
)";

    // ── Маршрут ──────────────────────────────────────────
    h += "<h2>🗺 Оптимальный маршрут покупок</h2>";
    h += "<h3>Порядок посещения</h3><p>";

    auto byStore = route.byStore();
    for (int i = 0; i < route.storeVisitOrder.size(); ++i) {
        int sid = route.storeVisitOrder[i];
        const Store* st = findStore(sid);
        QString name = st ? st->name : QString("Магазин %1").arg(sid);
        const QString safeName = name.toHtmlEscaped();

        h += QString("<span class='step'>%1</span><b>%2</b>").arg(i+1).arg(safeName);
        if (st && !st->address.isEmpty())
            h += QString(" <span style='color:#7f8c8d;'>(%1)</span>").arg(st->address.toHtmlEscaped());
        if (st && st->loyalty.active)
            h += QString("<span class='badge'>🎁 %1</span>").arg(st->loyalty.name.toHtmlEscaped());

        // Источник данных
        QString src = (st && st->priceSource == PriceSource::Network) ? "🌐 сеть" : "📦 встр.";
        h += QString("<span class='src'>%1</span>").arg(src);

        if (i + 1 < route.storeVisitOrder.size()) h += " &rarr; ";
    }
    h += "</p>";

    // ── Детализация по магазинам ─────────────────────────
    h += "<h3>Что купить в каждом магазине</h3>";

    for (int i = 0; i < route.storeVisitOrder.size(); ++i) {
        int sid = route.storeVisitOrder[i];
        const Store* st = findStore(sid);
        const auto& items = byStore[sid];
        if (items.isEmpty()) continue;

        QString name = st ? st->name : QString("Магазин %1").arg(sid);
        const QString safeName = name.toHtmlEscaped();
        h += QString("<div class='block'>"
                     "<b>Шаг %1: %2</b>").arg(i+1).arg(safeName);
        if (st && !st->address.isEmpty())
            h += QString(" &mdash; <span style='color:#555;'>%1</span>").arg(st->address.toHtmlEscaped());
        if (st && st->loyalty.active)
            h += QString("<span class='badge'>🎁 %1 (-%2% / кэшбэк %3%)</span>")
                    .arg(st->loyalty.name.toHtmlEscaped())
                    .arg(st->loyalty.discountPct,0,'f',0)
                    .arg(st->loyalty.cashbackPct,0,'f',0);
        h += "</div>";

        h += "<table><tr>"
             "<th>Товар</th><th>Ед.</th>"
             "<th>Обычная цена</th><th>Цена с картой</th><th>Экономия</th>"
             "</tr>";

        double totOrig = 0, totFinal = 0;
        for (const BestBuy& b : items) {
            double sav = b.regularPrice - b.finalPrice;
            totOrig  += b.regularPrice;
            totFinal += b.finalPrice;
            h += QString("<tr>"
                         "<td>%1</td><td style='color:#7f8c8d;'>%2</td>"
                         "<td>%3 ₽</td>")
                    .arg(b.productName.toHtmlEscaped()).arg(b.unit.toHtmlEscaped())
                    .arg(b.regularPrice,0,'f',2);

            if (b.usedCard)
                h += QString("<td><b>%1 ₽</b> 💳</td>"
                             "<td class='green'>−%2 ₽</td>")
                        .arg(b.finalPrice,0,'f',2).arg(sav,0,'f',2);
            else
                h += QString("<td>%1 ₽</td><td style='color:#bbb;'>—</td>")
                        .arg(b.finalPrice,0,'f',2);

            h += "</tr>";
        }

        double sav = totOrig - totFinal;
        h += QString("<tr class='tot'>"
                     "<td colspan='2'>Итого в магазине</td>"
                     "<td>%1 ₽</td><td>%2 ₽</td><td>−%3 ₽</td></tr>")
                .arg(totOrig,0,'f',2).arg(totFinal,0,'f',2).arg(sav,0,'f',2);
        h += "</table>";
    }

    // ── Итоговая таблица ─────────────────────────────────
    h += "<h3>Сводка</h3>";
    h += "<table>"
         "<tr><th>Показатель</th><th>Значение</th></tr>";
    h += QString("<tr><td>Сумма без скидок</td><td>%1 ₽</td></tr>")
            .arg(route.totalRegular,0,'f',2);
    h += QString("<tr><td>Сумма с учётом лояльности</td><td><b>%1 ₽</b></td></tr>")
            .arg(route.totalFinal,0,'f',2);
    h += QString("<tr><td>Экономия</td><td class='green'>%1 ₽</td></tr>")
            .arg(route.totalSavings,0,'f',2);
    h += QString("<tr><td>Длина маршрута</td><td>~%1 усл.ед.</td></tr>")
            .arg(route.totalDistance,0,'f',1);
    h += QString("<tr><td>Магазинов в маршруте</td><td>%1</td></tr>")
            .arg(route.storeVisitOrder.size());
    h += "</table>";

    // ── Не найденные товары ──────────────────────────────
    if (!route.notFound.isEmpty()) {
        h += "<div class='warn'><b>⚠️ Не найдено ни в одном магазине:</b><br>";
        for (const QString& p : route.notFound) h += "• " + p.toHtmlEscaped() + "<br>";
        h += "</div>";
    }

    return h;
}
