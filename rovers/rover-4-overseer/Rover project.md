# My mobile platform project on DC motors

  I call it "rover", inspired my moon rover.

  That's not the first incarnation, current platform design is 2nd and
  the parts arrangement is 5th. 1st generation had four wheels and actually
  resembled moon rover. Maybe I should call this two-wheeler "chariot".


# Principal electronic parts

  * Motor board

    Motor board is commanded by text protocol, similar to G codes (I call
    it "M codes"). It gives no feedback (except that the commands are sent
    to motors and it is ready for new commands). So it has no consciousness.

  * User of motor board

    User of motor board (I call it "overseer") is another microcontroller
    board. It has accelerometer, so has feedback and can have consciousness.
    But currently it just connects to WiFi, receives commands for motor board,
    sends them and provides sampled accelerometer feedback.

    Protocol for sending is HTTP GET/POST and response is JSON.

  * You

    "You" is you (or me) or your program. You send M commands and receive
    JSON with sampled acceleration/rotation over timeframe. Your task is
    to make rover to go where you want and how you want. In real world.


# Real world "challenges":

  * Accelerometer data is noisy

    To get position you need distance. To get distance you need to sum
    (integrate) over velocity. To get velocity you need to integrate
    over acceleration. Stationary accelerometer values are roaming near
    zeroes. Some damping over feedback values is needed.

  * Dimensions does matter

    Say you are rotating chariot by doing 100/-100 for several seconds
    and getting 412 degrees per second rate of rotation. Knowing
    distance between wheels will let you figure out speed of motors for
    that move.

    Also measuring distance in body lengths makes more sense here.

    Knowing weight is not needed but nice to know. As you naturally has
    TWR (thrust/weight ratio) from acceleration you can get thrust from
    that.

  * DC motors are not exactly equal

    Giving 40/40% to motors may cause one motor to start moving but
    the other will remain stall.

  * Ground is not flat

    Making turn on inclined surface requires skewing values to motors,
    depending on orientation.

  * There are walls!

    You can bump into them even when you figured out how to move
    straight. Well, at least you can update your map that this
    coordinates are not reachable from that point with given speed
    and orientation vectors.

  * Traction is not equal

    Hope you can get the wheel out of that oil/ice/hole spot. You can
    update your map too.

  * Dynamic forces may cause lifting the third touch point

    Think about making two-wheel balancing cart by keeping center of
    mass projection under the wheel touch points. You can use spinning
    motion for that too.

Rudolf Kalman is your new friend. PIDs is your new area of expertise.


Design notes

* Two wheeler

  I would prefer one wheel but have troubles with making turns.

  Most people think that the natural arrangement is four wheels near
  corners. Like in combustion cars or RC cars. But both of them have
  steering and one real motor.

  With two motors you can do differential steering which is superior.

  Four wheels. First platform generation had four powered wheels. With
  14.4V Makita battery. That was overpowered thing, like a rabbit on
  steroids. I burned motor controller (before utilizing step-down
  circuit) and threw wheels doing skid turns (before screwing them to
  axles).

  You have differential steering but it introduces sideslip. That tires
  with coned cylinders on it should help but I guess they have their own
  issues.

  Four powered wheels will give you stability, not-annoying-look and
  more traction points. But mentioned problems will still exist and
  balancing motor power distribution is more complex.

  * Wheels near center of mass projection

    I wanna try to make it dynamically balanced.

* Two boards

  Yes, one board is enough but writing new code, testing and debugging
  is more than twice easier when functionality is modularized.
  Communication channel is new failure point and adds latency though.
