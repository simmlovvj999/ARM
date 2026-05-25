#pragma once
#include "models.h"
#include <QPair>    // FIX: явный include для QPair

/*
 * RouteOptimizer — строит оптимальный маршрут покупок.
 *
 * Алгоритм:
 *  1. Для каждого товара находим магазин с минимальной ценой (с учётом лояльности).
 *  2. Определяем множество магазинов, которые нужно посетить.
 *  3. Оптимизируем порядок обхода магазинов:
 *     a) Nearest Neighbor (жадный TSP) — строим начальный маршрут
 *     b) 2-opt улучшение — сокращаем длину маршрута
 */
class RouteOptimizer {
public:
    static ShoppingRoute optimize(const OptimizeRequest& req);

private:
    static void applyLoyalty(Store& store, bool useLoyalty);

    static QPair<int, double> bestStoreFor(const QString& product,
                                            const QVector<Store>& stores,
                                            bool useLoyalty);

    static QVector<int> nearestNeighbor(const QVector<int>& ids,
                                         const QVector<Store>& stores);

    static QVector<int> twoOpt(QVector<int> route,
                                const QVector<Store>& stores);

    static double routeLen(const QVector<int>& route,
                            const QVector<Store>& stores);

    static const Store* storeById(int id, const QVector<Store>& stores);
};
