#include <pixel_format.h>

#ifdef HAS_FBDEV
#   define FBDEV_FORMAT_FIELD_INITIALIZER(r_length, r_offset, g_length, g_offset, b_length, b_offset, a_length, a_offset) \
        .fbdev_format = { \
            .r = {.length = r_length, .offset = r_offset, .msb_right = 0}, \
            .g = {.length = g_length, .offset = g_offset, .msb_right = 0}, \
            .b = {.length = b_length, .offset = b_offset, .msb_right = 0}, \
            .a = {.length = a_length, .offset = a_offset, .msb_right = 0}, \
        },
#else
#   define FBDEV_FORMAT_FIELD_INITIALIZER(r_length, r_offset, g_length, g_offset, b_length, b_offset, a_length, a_offset)
#endif

#ifdef HAS_GBM
#   include <gbm.h>
#   define GBM_FORMAT_FIELD_INITIALIZER(_gbm_format) .gbm_format = _gbm_format,
#else
#   define GBM_FORMAT_FIELD_INITIALIZER(_gbm_format)
#endif

#ifdef HAS_KMS
#   include <drm_fourcc.h>
#   define DRM_FORMAT_FIELD_INITIALIZER(_drm_format) .drm_format = _drm_format,
#else
#   define DRM_FORMAT_FIELD_INITIALIZER(_drm_format)
#endif

#define PIXFMT_MAPPING(_name, _arg_name, _format, _bpp, _is_opaque, r_length, r_offset, g_length, g_offset, b_length, b_offset, a_length, a_offset, _gbm_format, _drm_format) \
    { \
        .name = _name, \
        .arg_name = _arg_name, \
        .format = _format, \
        .bits_per_pixel = _bpp, \
        .is_opaque = _is_opaque, \
        FBDEV_FORMAT_FIELD_INITIALIZER(r_length, r_offset, g_length, g_offset, b_length, b_offset, a_length, a_offset) \
        GBM_FORMAT_FIELD_INITIALIZER(_gbm_format) \
        DRM_FORMAT_FIELD_INITIALIZER(_drm_format) \
    },

const struct pixfmt_info pixfmt_infos[] = {
    PIXFMT_LIST(PIXFMT_MAPPING)
};

/// hack so we can use COMPILE_ASSERT.
enum {
    n_pixfmt_infos_constexpr = sizeof(pixfmt_infos) / sizeof(*pixfmt_infos)
};

const size_t n_pixfmt_infos = n_pixfmt_infos_constexpr;

COMPILE_ASSERT(n_pixfmt_infos_constexpr == kMax_PixFmt+1);
