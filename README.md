# keycode
## [enigne](https://github.com/ferhatgec/enigne) based simple key macro program

### Example usage ([HOME]/.keycode): 

```py
if keycode_data equal_to "g_Release" =
 exec("xdg-open https://github.com/ferhatgec");
; elif keycode_data equal_to "q_Release" =
 exit();
; else =
 exec("aplay mechanical_keyboard_sound.flac");
;
```

### Back-end implementation/s available for:
 * X11

### keycode licensed under the terms of MIT License.
