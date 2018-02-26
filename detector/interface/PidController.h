#pragma once
#define BezierMagnitude 80.f
//double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint);
double bezier_calc(float wheelAngle, cv::Point2f &PointZero, cv::Point2f &PointOne, cv::Point2f &PointTwo, cv::Point2f &PointThree, double OutputArray[10]);
double set_steering_module(double wheelAngle);

class PIDController
{
public:
    PIDController(const float valMax,
                  const float valMin,
                  const float kp,
                  const float kd,
                  const float ki);
    PIDController(const PIDController& other);
    ~PIDController();

    float Calculate(const float setValue,
                    const float currentValue,
                    const float dt);
    
private:
    float m_valMax;
    float m_valMin;
    float m_kp;
    float m_kd;
    float m_ki;
    float m_previousError;
    float m_integral;
};