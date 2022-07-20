#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include <xkbcommon/xkbcommon.h>
#include <flutter_embedder.h>

#define MAX_COLLECTED_FLUTTER_POINTER_EVENTS 64

#define FLUTTER_POINTER_EVENT(_phase, _timestamp, _x, _y, _device, _signal_kind, _scroll_delta_x, _scroll_delta_y, _device_kind, _buttons) \
    (FlutterPointerEvent) { \
        .struct_size = sizeof(FlutterPointerEvent), \
        .phase = (_phase), \
        .timestamp = (_timestamp), \
        .x = (_x), .y = (_y), \
        .device = (_device), \
        .signal_kind = (_signal_kind), \
        .scroll_delta_x = (_scroll_delta_x), \
        .scroll_delta_y = (_scroll_delta_y), \
        .device_kind = (_device_kind), \
        .buttons = (_buttons) \
    }

#define FLUTTER_POINTER_TOUCH_ADD_EVENT(_timestamp, _x, _y, _device_id) \
    FLUTTER_POINTER_EVENT(kAdd, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindTouch, 0)

#define FLUTTER_POINTER_TOUCH_REMOVE_EVENT(_timestamp, _x, _y, _device_id) \
    FLUTTER_POINTER_EVENT(kRemove, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindTouch, 0)

#define FLUTTER_POINTER_TOUCH_MOVE_EVENT(_timestamp, _x, _y, _device_id) \
    FLUTTER_POINTER_EVENT(kMove, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindTouch, 0)

#define FLUTTER_POINTER_TOUCH_DOWN_EVENT(_timestamp, _x, _y, _device_id) \
    FLUTTER_POINTER_EVENT(kDown, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindTouch, 0)

#define FLUTTER_POINTER_TOUCH_UP_EVENT(_timestamp, _x, _y, _device_id) \
    FLUTTER_POINTER_EVENT(kUp, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindTouch, 0)

#define FLUTTER_POINTER_MOUSE_BUTTON_EVENT(_phase, _timestamp, _x, _y, _device_id, _buttons) \
    FLUTTER_POINTER_EVENT(_phase, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindMouse, _buttons)

#define FLUTTER_POINTER_MOUSE_ADD_EVENT(_timestamp, _x, _y, _device_id, _buttons) \
    FLUTTER_POINTER_EVENT(kAdd, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindMouse, _buttons)

#define FLUTTER_POINTER_MOUSE_SCROLL_EVENT(_timestamp, _x, _y, _device_id, _scroll_x, _scroll_y, _buttons) \
    FLUTTER_POINTER_EVENT(((_buttons) != 0) ? kMove : kHover, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindScroll, _scroll_x, _scroll_y, kFlutterPointerDeviceKindMouse, _buttons)

#define FLUTTER_POINTER_MOUSE_REMOVE_EVENT(_timestamp, _x, _y, _device_id, _buttons) \
    FLUTTER_POINTER_EVENT(kRemove, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindMouse, _buttons)

#define FLUTTER_POINTER_REMOVE_EVENT(_timestamp, _x, _y, _device, _buttons) \
    FLUTTER_POINTER_EVENT(kRemove, _timestamp, _x, _y, _device_id, kFlutterPointerSignalKindNone, 0.0, 0.0, kFlutterPointerDeviceKindMouse, _buttons)

#define FLUTTER_POINTER_MOUSE_MOVE_EVENT(_timestamp, _x, _y, _device_id, _buttons) \
    FLUTTER_POINTER_EVENT( \
        (_buttons) & kFlutterPointerButtonMousePrimary ? kMove : kHover, \
        _timestamp, \
        _x, _y, \
        _device_id, \
        kFlutterPointerSignalKindNone, \
        0.0, 0.0, \
        kFlutterPointerDeviceKindMouse, \
        _buttons\
    )

typedef void (*flutter_pointer_event_callback_t)(void *userdata, const FlutterPointerEvent *events, size_t n_events);

typedef void (*utf8_character_callback_t)(void *userdata, uint8_t *character);

typedef void (*xkb_keysym_callback_t)(void *userdata, xkb_keysym_t keysym);

typedef void (*gtk_keyevent_callback_t)(
	void *userdata,
	uint32_t unicode_scalar_values,
    uint32_t key_code,
    uint32_t scan_code,
    uint32_t modifiers,
    bool is_down
);

typedef void (*set_cursor_enabled_callback_t)(void *userdata, bool enabled);

typedef void (*move_cursor_callback_t)(void *userdata, unsigned int x, unsigned int y);

struct user_input_interface {
    flutter_pointer_event_callback_t on_flutter_pointer_event;
    utf8_character_callback_t on_utf8_character;
    xkb_keysym_callback_t on_xkb_keysym;
    gtk_keyevent_callback_t on_gtk_keyevent;
    set_cursor_enabled_callback_t on_set_cursor_enabled;
    move_cursor_callback_t on_move_cursor;
};

struct user_input;

/**
 * @brief Create a new user input instance. Will try to load the default keyboard config from /etc/default/keyboard
 * and create a udev-backed libinput instance.
 */
struct user_input *user_input_new(
    const struct user_input_interface *interface, 
    void *userdata,
    const FlutterTransformation *display_to_view_transform,
    const FlutterTransformation *view_to_display_transform,
	unsigned int display_width,
	unsigned int display_height
);

/**
 * @brief Destroy this user input instance and free all allocated memory. This will not remove any input devices
 * added to flutter and won't invoke any callbacks in the user input interface at all.
 */
void user_input_destroy(struct user_input *input);

/**
 * @brief Set a 3x3 matrix and display width / height so user_input can transform any device coordinates into
 * proper flutter view coordinates. (For example to account for a rotated display)
 * Will also transform absolute & relative mouse movements.
 * 
 * @param display_to_view_transform will be copied internally.
 */
void user_input_set_transform(
    struct user_input *input,
    const FlutterTransformation *display_to_view_transform,
    const FlutterTransformation *view_to_display_transform,
    unsigned int display_width,
    unsigned int display_height
);

/**
 * @brief Returns a filedescriptor used for input event notification. The returned
 * filedescriptor should be listened to with EPOLLIN | EPOLLRDHUP | EPOLLPRI or equivalent.
 * When the fd becomes ready, @ref user_input_on_fd_ready should be called not long after it
 * became ready. (libinput somehow relies on that)
 */
int user_input_get_fd(struct user_input *input);

/**
 * @brief Should be called when the fd returned by @ref user_input_get_fd becomes ready.
 * The user_input_interface callbacks will be called inside this function.
 */
int user_input_on_fd_ready(struct user_input *input);

#endif