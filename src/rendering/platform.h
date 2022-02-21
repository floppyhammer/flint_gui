//
// Created by chy on 2/16/2022.
//

#ifndef FLINT_PLATFORM_H
#define FLINT_PLATFORM_H


class Platform {
    static Platform &getSingleton() {
        static Platform singleton;
        return singleton;
    }
};


#endif //FLINT_PLATFORM_H
