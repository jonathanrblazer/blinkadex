# blinkadex


PWM:

docker build -t pwm-blink:dev .

docker run -it \
  --rm \
  --privileged \
  -v /sys/class/pwm:/sys/class/pwm \
  pwm-blink:dev
