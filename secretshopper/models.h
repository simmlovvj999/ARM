#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>
#include <cmath>

// ─────────────────────────────────────────────
//  Источник цен
// ─────────────────────────────────────────────
enum class PriceSource {
    Mock,       // встроенные демо-данные
    Network,    // HTTP-запрос к сайту/API
    Manual      // ручной ввод (резерв)
};

// ─────────────────────────────────────────────
//  Программа лояльности
// ─────────────────────────────────────────────
struct LoyaltyProgram {
    bool    active              = false;
    QString name;
    double  discountPct         = 0.0;   // % скидки по карте
    double  cashbackPct         = 0.0;   // % кэшбэка (вычитается из финальной суммы)
    double  minOrderForDiscount = 0.0;   // порог для активации скидки
    QString cardUrl;                     // ссылка на регистрацию карты
};

// ─────────────────────────────────────────────
//  Товар в конкретном магазине
// ─────────────────────────────────────────────
struct ProductPrice {
    QString name;
    double  price           = 0.0;
    double  priceDiscounted = 0.0;  // цена с картой лояльности
    bool    inStock         = false;
    QString unit;                   // "шт", "кг", "л" и т.д.
    QString imageUrl;
    QString productUrl;

    static ProductPrice fromJson(const QJsonObject& j) {
        ProductPrice p;
        p.name           = j["name"].toString();
        p.price          = j["price"].toDouble();
        p.priceDiscounted= j.contains("price_card") ? j["price_card"].toDouble() : p.price;
        p.inStock        = j["in_stock"].toBool(true);
        p.unit           = j["unit"].toString("шт");
        p.imageUrl       = j["image_url"].toString();
        p.productUrl     = j["product_url"].toString();
        return p;
    }
};

// ─────────────────────────────────────────────
//  Магазин
// ─────────────────────────────────────────────
struct Store {
    int     id          = 0;
    QString name;
    QString chain;      // сеть: "Пятёрочка", "Магнит", ...
    QString address;
    QString city;
    QString apiEndpoint;    // URL для получения цен (или пусто → mock)
    QPointF coords;         // (longitude, latitude) или условные (x, y)
    LoyaltyProgram loyalty;
    PriceSource priceSource = PriceSource::Mock;

    // Загруженные цены: productName → ProductPrice
    QMap<QString, ProductPrice> prices;

    bool pricesLoaded = false;
    QString lastError;

    double distanceTo(const Store& other) const {
        double dx = coords.x() - other.coords.x();
        double dy = coords.y() - other.coords.y();
        return std::sqrt(dx * dx + dy * dy);
    }
};

// ─────────────────────────────────────────────
//  Позиция оптимальной покупки
// ─────────────────────────────────────────────
struct BestBuy {
    QString productName;
    int     storeId         = -1;
    QString storeName;
    double  regularPrice    = 0.0;
    double  finalPrice      = 0.0;   // с учётом всех скидок
    bool    usedCard        = false;
    bool    inStock         = true;
    QString unit;
};

// ─────────────────────────────────────────────
//  Итоговый маршрут покупок
// ─────────────────────────────────────────────
struct ShoppingRoute {
    QVector<int>     storeVisitOrder;   // id магазинов в порядке обхода
    QVector<BestBuy> purchases;

    double totalRegular  = 0.0;
    double totalFinal    = 0.0;
    double totalSavings  = 0.0;
    double totalDistance = 0.0;        // условных единиц

    QVector<QString> notFound;         // товары, которых нет ни в одном магазине

    // Покупки, сгруппированные по магазину
    QMap<int, QVector<BestBuy>> byStore() const {
        QMap<int, QVector<BestBuy>> res;
        for (const auto& b : purchases) res[b.storeId].append(b);
        return res;
    }

    bool isEmpty() const { return purchases.isEmpty() && notFound.isEmpty(); }
};

// ─────────────────────────────────────────────
//  Запрос на оптимизацию
// ─────────────────────────────────────────────
struct OptimizeRequest {
    QVector<QString> products;
    QVector<Store>   stores;
    bool useLoyalty    = true;
    bool optimizeRoute = true;
};
