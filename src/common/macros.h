#ifndef FLINT_MACROS_H
#define FLINT_MACROS_H

#define VK_CHECK_RESULT(f)                                                                               \
{                                                                                                        \
    VkResult res = (f);                                                                                  \
    if (res != VK_SUCCESS) {                                                                             \
        std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
        assert(res == VK_SUCCESS);                                                                       \
    }                                                                                                    \
}

#endif //FLINT_MACROS_H
