#pragma once
#include "models.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QTimer>

/*
 * PriceService — сервис получения цен.
 *
 * Поддерживает два режима работы:
 *  1. Mock    — ответ из встроенной базы данных (MockPriceServer)
 *  2. Network — реальный HTTP GET-запрос к JSON-API магазина
 *
 * Ожидаемый формат JSON-ответа:
 *  {
 *    "store": "Название",
 *    "products": [
 *      { "name": "Молоко", "price": 89.99, "price_card": 85.49,
 *        "in_stock": true, "unit": "1 л" },
 *      ...
 *    ]
 *  }
 */
class PriceService : public QObject {
    Q_OBJECT
public:
    explicit PriceService(QObject* parent = nullptr);

    // Загружает цены для всех магазинов; по завершению эмитирует allLoaded()
    void fetchPricesForAll(QVector<Store>& stores,
                           const QStringList& products);

    // Загружает цены для одного магазина
    void fetchForStore(Store& store, const QStringList& products);

signals:
    void storeLoaded(int storeId, bool success, const QString& errorMsg);
    void allLoaded(int successCount, int failCount);
    void progress(int pct);

private:
    // FIX: убран незадекларированный слот onNetworkReply, которого не было в .cpp
    bool parseJsonResponse(const QByteArray& data,
                           Store& store,
                           const QStringList& products);

    bool loadMock(Store& store, const QStringList& products);

    QNetworkAccessManager* m_nam;

    int m_pending = 0;
    int m_success = 0;
    int m_fail    = 0;
};
