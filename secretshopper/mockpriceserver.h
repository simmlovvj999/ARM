#pragma once
#include "models.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>    // FIX: явный include для QStringList
#include <QMap>
#include <QVector>

/*
 * MockPriceServer — встроенная база цен.
 * Данные подобраны приближённо к реальным ценам 2025 г.
 * Формат ответа имитирует JSON-API: { "store": "...", "products": [...] }
 */
class MockPriceServer {
public:
    static QJsonDocument getPrices(const QString& chain,
                                    const QStringList& productNames);

    static QStringList supportedChains() {
        return {"Пятёрочка", "Магнит", "Перекрёсток", "ВкусВилл", "Лента",
                "Дикси", "Ашан", "Metro"};
    }

    static LoyaltyProgram loyaltyFor(const QString& chain);

    static QPointF defaultCoords(int storeId) {
        static const QVector<QPointF> coords = {
            {1.0, 1.0}, {4.5, 2.0}, {2.5, 5.0},
            {6.0, 1.5}, {0.5, 4.0}, {5.5, 4.5},
        };
        return coords[storeId % coords.size()];
    }

private:
    struct PriceEntry { double price; double priceCard; QString unit; };
    using ChainPrices = QMap<QString, PriceEntry>;

    static QMap<QString, ChainPrices> buildDatabase();
    static QMap<QString, ChainPrices> s_db;
};
