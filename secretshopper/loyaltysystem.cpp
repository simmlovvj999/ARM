#include "loyaltysystem.h"
#include "mockpriceserver.h"

LoyaltyProgram LoyaltySystem::detect(const QString& chainName) {
    return MockPriceServer::loyaltyFor(chainName);
}
