#include "priceservice.h"
#include "mockpriceserver.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QUrl>

PriceService::PriceService(QObject* parent)
    : QObject(parent),
      m_nam(new QNetworkAccessManager(this))
{}

void PriceService::fetchPricesForAll(QVector<Store>& stores,
                                      const QStringList& products)
{
    m_pending = stores.size();
    m_success = 0;
    m_fail    = 0;

    if (m_pending == 0) {
        emit allLoaded(0, 0);
        return;
    }

    for (Store& store : stores) {
        fetchForStore(store, products);
    }
}

void PriceService::fetchForStore(Store& store, const QStringList& products)
{
    if (store.priceSource == PriceSource::Mock || store.apiEndpoint.isEmpty()) {
        Store* storePtr = &store;
        int    storeId  = store.id;

        QTimer::singleShot(80 + storeId * 30, this, [this, storePtr, products]() {
            const bool ok = loadMock(*storePtr, products);
            storePtr->pricesLoaded = ok;
            if (ok)
                ++m_success;
            else
                ++m_fail;

            int total = m_success + m_fail;
            if (m_pending > 0)
                emit progress(total * 100 / m_pending);
            emit storeLoaded(storePtr->id, ok, ok ? QString() : storePtr->lastError);

            if (total >= m_pending)
                emit allLoaded(m_success, m_fail);
        });

    } else {
        QNetworkRequest req(QUrl(store.apiEndpoint));
        req.setRawHeader("User-Agent",
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
            "AppleWebKit/537.36 Chrome/120 Safari/537.36");
        req.setRawHeader("Accept", "application/json");

        QNetworkReply* reply = m_nam->get(req);

        Store* storePtr = &store;
        int    sid      = store.id;

        connect(reply, &QNetworkReply::finished,
                this, [this, reply, sid, storePtr, products]() {
            bool ok = false;
            if (reply->error() != QNetworkReply::NoError) {
                storePtr->lastError = reply->errorString();
                ok = loadMock(*storePtr, products);
            } else {
                const QByteArray data = reply->readAll();
                ok = parseJsonResponse(data, *storePtr, products);
                if (!ok)
                    ok = loadMock(*storePtr, products);
            }
            storePtr->pricesLoaded = ok;
            reply->deleteLater();

            if (ok)
                ++m_success;
            else
                ++m_fail;
            int total = m_success + m_fail;
            if (m_pending > 0)
                emit progress(total * 100 / m_pending);
            emit storeLoaded(sid, ok, storePtr->lastError);

            if (total >= m_pending)
                emit allLoaded(m_success, m_fail);
        });
    }
}

bool PriceService::parseJsonResponse(const QByteArray& data,
                                      Store& store,
                                      const QStringList& products)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        store.lastError = "Ошибка разбора JSON: " + err.errorString();
        Q_UNUSED(products);
        return false;
    }

    QJsonObject root = doc.object();
    if (root.contains("error")) {
        store.lastError = root["error"].toString("Источник цен вернул ошибку");
        return false;
    }

    QJsonArray  arr  = root["products"].toArray();
    if (arr.isEmpty() && !products.isEmpty()) {
        store.lastError = "Источник цен не вернул товары из списка";
        return false;
    }

    store.prices.clear();
    for (const QJsonValue& v : arr) {
        QJsonObject  obj = v.toObject();
        ProductPrice pp  = ProductPrice::fromJson(obj);
        if (!pp.name.isEmpty()) {
            store.prices[pp.name] = pp;
        }
    }

    return true;
}

bool PriceService::loadMock(Store& store, const QStringList& products)
{
    QJsonDocument doc = MockPriceServer::getPrices(store.chain, products);
    if (doc.isNull() || !parseJsonResponse(doc.toJson(), store, products)) {
        store.prices.clear();
        return false;
    }
    return true;
}
