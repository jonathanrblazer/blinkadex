# blinkadex


PWM:

docker build -t pwm-blink:dev .

docker run -it \
  --rm \
  --privileged \
  -v /sys/class/pwm:/sys/class/pwm \
  pwm-blink:dev


torizon@verdin-am62-15600644:~$ docker run --rm -it --privileged \
> -v /sys/class/pwm:/sys/class/pwm \
> torizonextras/arm64v8-gpiod

root@1c4bb6ed6a81:/# ls /sys/class/pwm
pwmchip0

root@1c4bb6ed6a81:/# cat /sys/class/pwm/pwmchip0/npwm
2

root@1c4bb6ed6a81:/# echo 0 | sudo tee /sys/class/pwm/pwmchip0/export
0

root@1c4bb6ed6a81:/# ls /sys/class/pwm/pwmchip0/
device  export  npwm  power  pwm0  subsystem  uevent  unexport

BUG 1/17/2026 15:30
