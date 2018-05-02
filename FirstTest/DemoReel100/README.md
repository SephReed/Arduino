# HOW TO SETUP

1. Install Arduino IDE (https://www.arduino.cc/en/Main/Donate)

2. Open Arduino IDE

3. Download Code (https://github.com/SephReed/Arduino/blob/master/FirstTest/DemoReel100/DemoReel100.ino)

4. Download TeensyDuino (https://www.pjrc.com/teensy/td_download.html)

5. Run TeensyDuino, following it's instructions.  Make sure to open Arduino IDE first, or else you'll break it's auth token.

6. To connect the teensy duino try on your own, then call nick or seph.



## Easy Modifications to Code

These three button pin ids are not yet properly set:

```
#define NEW_PATTERN_BUTTON_PIN          4
#define REVERSE_MODE_BUTTON_PIN         5
#define FLIP_HUE_BUTTON_PIN             6
```

To change the overall brightness, modify this
```
#define OVERALL_BRIGHTNESS  60
```


That's all.