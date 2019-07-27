#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iterator>
#include <variant>
#include <vector>

#include <iostream>

namespace license_calculator {
class TooFewArgumentError : public std::exception {
   public:
    const char *what() const throw() { return "relations must have at least 2 arguments"; }
};
class MissingPairError : public std::exception {
    const char *what() const throw() { return "relation doesn't have matching begin-end pair"; }
};

/**
 * @brief used to begin and end license description
 *
 */
enum class LicenseRelation { ALL_LICENSE_BELOW, EITHER_LICENSE_BELOW, END };

/**
 * @brief Type to describe days in epoch seconds
 *
 * Change this to match the type you use
 */
typedef int ExpirationDayType;

namespace internal {
using LicenseStackElem = std::variant<LicenseRelation, ExpirationDayType>;

void handleRelation(std::vector<LicenseStackElem> &licenseStack, const LicenseRelation &relation) {
    if (relation != LicenseRelation::END) {
        licenseStack.emplace_back(relation);
    } else {
        auto begin = licenseStack.rbegin();
        auto end   = licenseStack.rbegin();

        auto i = licenseStack.rbegin();
        while (std::holds_alternative<ExpirationDayType>(*i)) { ++i; }
        end = i;

        if (i == licenseStack.rend()) { throw MissingPairError(); }

        if ((begin == end) || (((begin + 1) == end) && (end != (licenseStack.rend() - 1)))) {
            throw TooFewArgumentError();
        }

        LicenseStackElem res;

        switch (std::get<LicenseRelation>(*end)) {
            case LicenseRelation::ALL_LICENSE_BELOW:
                res = *(std::min_element(
                    begin, end, [](const LicenseStackElem &lhs, const LicenseStackElem &rhs) {
                        return std::get<ExpirationDayType>(lhs) < std::get<ExpirationDayType>(rhs);
                    }));
                break;
            case LicenseRelation::EITHER_LICENSE_BELOW:
                res = *(std::max_element(
                    begin, end, [](const LicenseStackElem &lhs, const LicenseStackElem &rhs) {
                        return std::get<ExpirationDayType>(lhs) < std::get<ExpirationDayType>(rhs);
                    }));
                break;
            default:
                throw std::runtime_error("unknown license relation type");
                break;
        }

        licenseStack.erase((end + 1).base(), (begin).base());
        licenseStack.emplace_back(res);
    }
}

template <typename... Args>
ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack,
                          const LicenseRelation &        relation,
                          Args... args);
ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack,
                          const LicenseRelation &        relation);

ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack, ExpirationDayType &day) {
    throw MissingPairError();
    return 0;
}
template <typename... Args>
ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack,
                          ExpirationDayType &            day,
                          Args... args) {
    licenseStack.emplace_back(day);
    return calcDay(licenseStack, args...);
}

ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack,
                          const LicenseRelation &        relation) {
    if (relation != LicenseRelation::END) { throw MissingPairError(); }
    handleRelation(licenseStack, relation);
    return std::get<ExpirationDayType>(licenseStack.front());
}
template <typename... Args>
ExpirationDayType calcDay(std::vector<LicenseStackElem> &licenseStack,
                          const LicenseRelation &        relation,
                          Args... args) {
    handleRelation(licenseStack, relation);
    return calcDay(licenseStack, args...);
}
}  // namespace internal

template <typename... Args>
ExpirationDayType calculateExpirationDay(Args... args) {
    std::vector<internal::LicenseStackElem> licenseStack;
    return internal::calcDay(licenseStack, args...);
}
}  // namespace license_calculator