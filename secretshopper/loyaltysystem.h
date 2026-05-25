#pragma once
#include "models.h"
#include <QString>

class LoyaltySystem {
public:
    static LoyaltyProgram detect(const QString& chainName);
};
