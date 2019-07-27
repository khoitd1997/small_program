# License Calculator

A very small DSL for calculating days until license expiration using C++ varidic templates. There are 2 types of license relations:

- ```ALL_LICENSE_BELOW```: The min of all licenses below will be calculated
- ```EITHER_LICENSE_BELOW```: The max of all licenses below will be calculated

Every relation will be terminated with ```END```, relations **can be nested**

Check ```main.cpp``` for tests and examples, but basically to calculate license days left:

```cpp
#include "license_calculator.hpp"
const auto res = calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
    7,
    5,
        LicenseRelation::EITHER_LICENSE_BELOW,
        1,
        2,
        LicenseRelation::END,
    LicenseRelation::END
);

// one member is allowed only for outermost license
const auto res = calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
    7,
    LicenseRelation::END
);

/*** CASES THAT WILL THROW EXCEPTIONS ***/
calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
    LicenseRelation::END
);

calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
);

calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
    1,
        LicenseRelation::ALL_LICENSE_BELOW,
        2,
        LicenseRelation::END,
    LicenseRelation::END
);

calculateExpirationDay(
    LicenseRelation::ALL_LICENSE_BELOW,
    1,
        LicenseRelation::ALL_LICENSE_BELOW,
        LicenseRelation::END,
    LicenseRelation::END
);
```
