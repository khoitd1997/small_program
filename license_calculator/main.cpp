#include <iostream>

#include <cassert>
#include "license_calculator.hpp"

using namespace license_calculator;
int main(void) {
    {
        const auto res = calculateExpirationDay(
            // clang-format off
            LicenseRelation::ALL_LICENSE_BELOW,
            30,
            15,
            LicenseRelation::END
            // clang-format on
        );
        assert(15 == res);
    }

    {
        const auto res = calculateExpirationDay(
            // clang-format off
            LicenseRelation::EITHER_LICENSE_BELOW,
            100,
            5,
            LicenseRelation::END
            // clang-format on
        );
        assert(100 == res);
    }

    {
        const auto res = calculateExpirationDay(
            // clang-format off
            LicenseRelation::EITHER_LICENSE_BELOW,
            1,
            5,
                LicenseRelation::ALL_LICENSE_BELOW,
                100,
                200,
                LicenseRelation::END,
            LicenseRelation::END
            // clang-format on
        );
        assert(100 == res);
    }

    {
        const auto res = calculateExpirationDay(
            // clang-format off
            LicenseRelation::ALL_LICENSE_BELOW,
            7,
            5,
                LicenseRelation::EITHER_LICENSE_BELOW,
                1,
                2,
                LicenseRelation::END,
            LicenseRelation::END
            // clang-format on
        );
        assert(2 == res);
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                LicenseRelation::ALL_LICENSE_BELOW,
                LicenseRelation::END
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of outer empty case" << std::endl;
        } catch (const TooFewArgumentError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed outer empty case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                LicenseRelation::ALL_LICENSE_BELOW,
                1,
                2,
                    LicenseRelation::ALL_LICENSE_BELOW,
                    LicenseRelation::END,
                LicenseRelation::END
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of inner empty case" << std::endl;
        } catch (const TooFewArgumentError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed inner empty case" << std::endl; }
    }

    {
        // 1 arg is allowed only for outermost relation
        try {
            const auto res = calculateExpirationDay(
                // clang-format off
                LicenseRelation::ALL_LICENSE_BELOW,
                42,
                LicenseRelation::END
                // clang-format on
            );
            assert(res == 42);
        } catch (...) { std::cerr << "failed 1 arg outer case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                LicenseRelation::ALL_LICENSE_BELOW,
                1,
                    LicenseRelation::ALL_LICENSE_BELOW,
                    2,
                    LicenseRelation::END,
                LicenseRelation::END
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of 1 arg inner case" << std::endl;
        } catch (const TooFewArgumentError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed 1 arg inner case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                LicenseRelation::ALL_LICENSE_BELOW
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of failed missing case" << std::endl;
        } catch (const MissingPairError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed missing argument case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                LicenseRelation::END
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of failed missing case" << std::endl;
        } catch (const MissingPairError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed missing argument case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                1,
                2
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of failed missing case" << std::endl;
        } catch (const MissingPairError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed missing argument case" << std::endl; }
    }

    {
        try {
            calculateExpirationDay(
                // clang-format off
                1,
                2,
                LicenseRelation::END
                // clang-format on
            );
            std::cerr << "shouldn't have reached end of failed missing case" << std::endl;
        } catch (const MissingPairError& e) {
            // the correct case
        } catch (...) { std::cerr << "failed missing argument case" << std::endl; }
    }

    return 0;
}