#include "routeoptimizer.h"
#include "models.h"

#include <QSet>
#include <QMap>
#include <QtMath>
#include <algorithm>
#include <limits>

// ── Публичный статический метод ──────────────────────────
ShoppingRoute RouteOptimizer::optimize(const OptimizeRequest& req) {
    ShoppingRoute result;

    if (req.products.isEmpty() || req.stores.isEmpty()) {
        return result;
    }

    // 1. Для каждого товара находим лучший магазин
    // товар → {лучший_магазин, лучшая_цена, использована_ли_карта}
    QMap<QString, BestBuy> bestChoices;

    for (int i = 0; i < req.products.size(); ++i) {
        const QString& productName = req.products[i];

        QPair<int, double> best = bestStoreFor(productName, req.stores, req.useLoyalty);
        int bestStoreId = best.first;
        double bestPrice = best.second;

        if (bestStoreId >= 0 && bestPrice > 0) {
            // Находим магазин для заполнения данных
            const Store* store = storeById(bestStoreId, req.stores);
            if (store && store->prices.contains(productName)) {
                const ProductPrice& pp = store->prices[productName];

                BestBuy buy;
                buy.productName = productName;
                buy.storeId = store->id;
                buy.storeName = store->name;
                buy.regularPrice = pp.price;
                buy.finalPrice = bestPrice;
                buy.usedCard = (req.useLoyalty && bestPrice == pp.priceDiscounted && pp.priceDiscounted < pp.price);
                buy.inStock = pp.inStock;
                buy.unit = pp.unit;

                bestChoices[productName] = buy;
                result.purchases.append(buy);
            }
        } else {
            // Товар не найден ни в одном магазине
            result.notFound.append(productName);
        }
    }

    // 2. Собираем уникальные магазины для посещения
    QSet<int> uniqueStores;
    for (int i = 0; i < result.purchases.size(); ++i) {
        uniqueStores.insert(result.purchases[i].storeId);
    }
    result.storeVisitOrder = QVector<int>::fromList(uniqueStores.values());
    std::sort(result.storeVisitOrder.begin(), result.storeVisitOrder.end());

    // 3. Оптимизируем порядок обхода (если включено и есть координаты)
    if (req.optimizeRoute && result.storeVisitOrder.size() > 1) {
        result.storeVisitOrder = nearestNeighbor(result.storeVisitOrder, req.stores);
        result.storeVisitOrder = twoOpt(result.storeVisitOrder, req.stores);
    }

    // 4. Считаем итоговые суммы
    result.totalRegular = 0.0;
    result.totalFinal = 0.0;

    for (int i = 0; i < result.purchases.size(); ++i) {
        const BestBuy& buy = result.purchases[i];
        result.totalRegular += buy.regularPrice;
        result.totalFinal += buy.finalPrice;
    }

    result.totalSavings = result.totalRegular - result.totalFinal;
    if (result.totalSavings < 0.0) {
        result.totalSavings = 0.0;
    }

    // 5. Считаем общую дистанцию маршрута
    result.totalDistance = 0.0;
    for (int i = 0; i < result.storeVisitOrder.size() - 1; ++i) {
        const Store* a = storeById(result.storeVisitOrder[i], req.stores);
        const Store* b = storeById(result.storeVisitOrder[i + 1], req.stores);
        if (a && b) {
            result.totalDistance += a->distanceTo(*b);
        }
    }

    return result;
}

// ── Приватные вспомогательные методы ────────────────────

void RouteOptimizer::applyLoyalty(Store& store, bool useLoyalty) {
    // В данной архитектуре цены уже содержат priceDiscounted,
    // поэтому просто ничего не делаем — логика в bestStoreFor
    Q_UNUSED(store);
    Q_UNUSED(useLoyalty);
}

QPair<int, double> RouteOptimizer::bestStoreFor(const QString& productName,
                                                 const QVector<Store>& stores,
                                                 bool useLoyalty) {
    int bestId = -1;
    double bestPrice = std::numeric_limits<double>::max();

    for (int s = 0; s < stores.size(); ++s) {
        const Store& store = stores[s];

        // Проверяем, есть ли товар в этом магазине
        if (!store.prices.contains(productName)) {
            continue;
        }

        const ProductPrice& pp = store.prices[productName];

        // Товар должен быть в наличии
        if (!pp.inStock) {
            continue;
        }

        // Выбираем цену: с картой лояльности или обычную
        double price = useLoyalty ? pp.priceDiscounted : pp.price;
        if (price <= 0) {
            price = pp.price; // fallback, если priceDiscounted не задан
        }

        // Обновляем лучшую цену
        if (price > 0 && price < bestPrice) {
            bestPrice = price;
            bestId = store.id;
        }
    }

    return QPair<int, double>(bestId, bestPrice);
}

QVector<int> RouteOptimizer::nearestNeighbor(const QVector<int>& ids,
                                              const QVector<Store>& stores) {
    if (ids.size() <= 1) {
        return ids;
    }

    QVector<int> route;
    QSet<int> visited;

    // Начинаем с первого магазина
    route.append(ids.first());
    visited.insert(ids.first());

    // Жадный алгоритм: всегда идём к ближайшему непосещённому
    while (visited.size() < ids.size()) {
        int currentId = route.last();
        const Store* current = storeById(currentId, stores);
        if (!current) {
            break;
        }

        int nearestId = -1;
        double minDist = std::numeric_limits<double>::max();

        for (int i = 0; i < ids.size(); ++i) {
            int candidateId = ids[i];
            if (visited.contains(candidateId)) {
                continue;
            }

            const Store* candidate = storeById(candidateId, stores);
            if (!candidate) {
                continue;
            }

            double dist = current->distanceTo(*candidate);
            if (dist < minDist) {
                minDist = dist;
                nearestId = candidateId;
            }
        }

        if (nearestId >= 0) {
            route.append(nearestId);
            visited.insert(nearestId);
        } else {
            break; // нет доступных магазинов
        }
    }

    return route;
}

QVector<int> RouteOptimizer::twoOpt(QVector<int> route,
                                     const QVector<Store>& stores) {
    if (route.size() <= 2) {
        return route;
    }

    bool improved = true;
    int iterations = 0;
    const int maxIterations = 100; // защита от бесконечного цикла

    while (improved && iterations < maxIterations) {
        improved = false;
        ++iterations;

        for (int i = 0; i < route.size() - 1; ++i) {
            for (int j = i + 1; j < route.size(); ++j) {
                // Пробуем развернуть участок [i, j]
                QVector<int> newRoute = route;
                std::reverse(newRoute.begin() + i, newRoute.begin() + j + 1);

                if (routeLen(newRoute, stores) < routeLen(route, stores)) {
                    route = newRoute;
                    improved = true;
                    break;
                }
            }
            if (improved) {
                break;
            }
        }
    }

    return route;
}

double RouteOptimizer::routeLen(const QVector<int>& route,
                                 const QVector<Store>& stores) {
    if (route.size() <= 1) {
        return 0.0;
    }

    double total = 0.0;
    for (int i = 0; i < route.size() - 1; ++i) {
        const Store* a = storeById(route[i], stores);
        const Store* b = storeById(route[i + 1], stores);
        if (a && b) {
            total += a->distanceTo(*b);
        }
    }
    return total;
}

const Store* RouteOptimizer::storeById(int id, const QVector<Store>& stores) {
    for (int i = 0; i < stores.size(); ++i) {
        if (stores[i].id == id) {
            return &stores[i];
        }
    }
    return nullptr;
}
