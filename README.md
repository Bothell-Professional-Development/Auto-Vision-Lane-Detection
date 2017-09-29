# Lane Detection and Autonomous Steering Using Computer Vision

## Abstract:
Develop lane detection software that finds lane markings in realtime from a digital video feed. May also be used to estimate current velocity. In conjunction use a PID control algorithm to course correct based upon constantly shifting lane markers in video.

## Scope:
Development of computer vision system specifically to detect lane markings only. Obstacles, including cars and pedestrians, will be ignored for the sake of simplicity. Demo will probably be static given timeframe, e.g. Will use prerecorded driving footage from a front grill perspective and demonstrate detection of lane markings and mock steering output to correct deviations from the center of the lane.

## Technical Hurdles:
Computer vision is hard. Will likely need to resort to starting with an off-the-shelf open-source solution and adapt for my use case rather than reinvent the wheel by myself.

## Stretch Goals:
1. Get software running at acceptable rate on an embedded device.
1. Integrate aforementioned embedded device into an RC car and use an Arduino or something to feed in inputs (PWM, PCM, etc.) into RC receiver.

## Timeline:
Now until end of review period?

## Project Team/Tentative Roles:
Patrick Daly, Engineering
Christos Palaskas, Engineering
Matthew White, Engineering/ Project Management /Scrum Master
Derek Kumagai, Engineering/Project Management

## Sprint Breakdown/Meetings?
-Biweekly Sprints
-Minimum of weekly meetings
-Sprint goals TBD

## Opensource Software:
opencv-3.3.0
