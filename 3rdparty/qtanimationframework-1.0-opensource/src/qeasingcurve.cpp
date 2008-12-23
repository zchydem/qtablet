/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

/*!
    \class QtEasingCurve
    \ingroup animation
    \brief The QtEasingCurve class provides easing curves for controlling animation.

    Easing curves describe a function that controls how the speed of the interpolation
    between 0 and 1 should be. Easing curves allow transitions from 
    one value to another to appear more natural than a simple constant speed would allow.
    The QtEasingCurve class is usually used in conjunction with the QtAnimation class, 
    but can be used on its own.

    To calculate the speed of the interpolation, the easing curve provides the function
    valueForStep(), where the \a step argument specifies the progress of the 
    interpolation: 0 is the start value of the interpolation, 1 is the end value of the 
    interpolation. The returned value is the effective progress of the interpolation.
    If the returned value is the same as the input value for all input values the easing
    curve is a linear curve. This is the default behaviour. 

    For example,
    \code
    QtEasingCurve easing(QtEasingCurve::InOutQuad);

    for(qreal step = 0.0; step < 1.0; step+=0.1)
        qWarning() << "Effective progress" << step << " is
                   << easing.valueForStep(step);
    \endcode
    will print the effective progress of the interpolation between 0 and 1.

    When using a QtAnimation, the easing curve will be used to control the
    progress of the interpolation between startValue and endValue:
    \code
    QtAnimation animation;
    animation.setStartValue(0);
    animation.setEndValue(1000);
    animation.setDuration(1000);
    animation.setEasing(QtEasingCurve(QtEasingCurve::InOutQuad));
    \endcode
 */

/*!
    \enum QtEasingCurve::Type

    The type of easing curve.

    \value Linear Easing equation function for a simple linear tweening, with no easing.
    \value InQuad Easing equation function for a quadratic (t^2) easing in: accelerating from zero velocity.
    \value OutQuad Easing equation function for a quadratic (t^2) easing out: decelerating to zero velocity.
    \value InOutQuad Easing equation function for a quadratic (t^2) easing in/out: acceleration until halfway, then deceleration.
    \value OutInQuad Easing equation function for a quadratic (t^2) easing out/in: deceleration until halfway, then acceleration.
    \value InCubic Easing equation function for a cubic (t^3) easing in: accelerating from zero velocity.
    \value OutCubic Easing equation function for a cubic (t^3) easing out: decelerating from zero velocity.
    \value InOutCubic Easing equation function for a cubic (t^3) easing in/out: acceleration until halfway, then deceleration.
    \value OutInCubic Easing equation function for a cubic (t^3) easing out/in: deceleration until halfway, then acceleration.
    \value InQuart Easing equation function for a quartic (t^4) easing in: accelerating from zero velocity.
    \value OutQuart Easing equation function for a quartic (t^4) easing out: decelerating from zero velocity.
    \value InOutQuart Easing equation function for a quartic (t^4) easing in/out: acceleration until halfway, then deceleration.
    \value OutInQuart Easing equation function for a quartic (t^4) easing out/in: deceleration until halfway, then acceleration.
    \value InQuint Easing equation function for a quintic (t^5) easing in: accelerating from zero velocity.
    \value OutQuint Easing equation function for a quintic (t^5) easing out: decelerating from zero velocity.
    \value InOutQuint Easing equation function for a quintic (t^5) easing in/out: acceleration until halfway, then deceleration.
    \value OutInQuint Easing equation function for a quintic (t^5) easing out/in: deceleration until halfway, then acceleration.
    \value InSine Easing equation function for a sinusoidal (sin(t)) easing in: accelerating from zero velocity.
    \value OutSine Easing equation function for a sinusoidal (sin(t)) easing out: decelerating from zero velocity.
    \value InOutSine Easing equation function for a sinusoidal (sin(t)) easing in/out: acceleration until halfway, then deceleration.
    \value OutInSine Easing equation function for a sinusoidal (sin(t)) easing out/in: deceleration until halfway, then acceleration.
    \value InExpo Easing equation function for an exponential (2^t) easing in: accelerating from zero velocity.
    \value OutExpo Easing equation function for an exponential (2^t) easing out: decelerating from zero velocity.
    \value InOutExpo Easing equation function for an exponential (2^t) easing in/out: acceleration until halfway, then deceleration.
    \value OutInExpo Easing equation function for an exponential (2^t) easing out/in: deceleration until halfway, then acceleration.
    \value InCirc Easing equation function for a circular (sqrt(1-t^2)) easing in: accelerating from zero velocity.
    \value OutCirc Easing equation function for a circular (sqrt(1-t^2)) easing out: decelerating from zero velocity.
    \value InOutCirc Easing equation function for a circular (sqrt(1-t^2)) easing in/out: acceleration until halfway, then deceleration.
    \value OutInCirc Easing equation function for a circular (sqrt(1-t^2)) easing out/in: deceleration until halfway, then acceleration.
    \value InElastic Easing equation function for an elastic (exponentially decaying sine wave) easing in: accelerating from zero velocity.  The peak amplitude can be set with the \i amplitude parameter, and the period of decay by the \i period parameter.
    \value OutElastic Easing equation function for an elastic (exponentially decaying sine wave) easing out: decelerating from zero velocity.  The peak amplitude can be set with the \i amplitude parameter, and the period of decay by the \i period parameter.
    \value InOutElastic Easing equation function for an elastic (exponentially decaying sine wave) easing in/out: acceleration until halfway, then deceleration.
    \value OutInElastic Easing equation function for an elastic (exponentially decaying sine wave) easing out/in: deceleration until halfway, then acceleration.
    \value InBack Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
    \value OutBack Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out: decelerating from zero velocity.
    \value InOutBack Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
    \value OutInBack Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
    \value OutBounce Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out: decelerating from zero velocity.
    \value InBounce Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in: accelerating from zero velocity.
    \value InOutBounce Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in/out: acceleration until halfway, then deceleration.
    \value OutInBounce Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out/in: deceleration until halfway, then acceleration.
    \omitvalue InCurve
    \omitvalue OutCurve
    \omitvalue SineCurve
    \omitvalue CosineCurve
    */

#include "qeasingcurve.h"
#include <QtCore/qmath.h>
#include <QtCore/qhash.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

typedef float (*Function)(float t, float b, float c, float d);

static bool isConfigFunction(QtEasingCurve::Type type)
{
    return type == QtEasingCurve::InElastic
            || type == QtEasingCurve::OutElastic
            || type == QtEasingCurve::InBounce
            || type == QtEasingCurve::OutBounce;
}

class QtEasingCurveFunction
{
public:
    enum Type { In, Out };

    QtEasingCurveFunction(QtEasingCurveFunction::Type type = In, qreal period = 0.3f, qreal amplitude = 1.0f)
        : _t(type), _p(period), _a(amplitude)
    { }
    virtual ~QtEasingCurveFunction() {}
    virtual float value(float t, float b, float c, float d);
    virtual QtEasingCurveFunction *copy() const;

    Type _t;
    qreal _p;
    qreal _a;
};

float QtEasingCurveFunction::value(float t, float b, float c, float d)
{
    return t;
}

QtEasingCurveFunction *QtEasingCurveFunction::copy() const
{
    return new QtEasingCurveFunction(_t, _p, _a);
}

#ifdef QT_EXPERIMENTAL_SOLUTION
# include "easing.cpp"
#else
# include "../3rdparty/easing/easing.cpp"
#endif

class QtEasingCurvePrivate
{
public:
    QtEasingCurvePrivate()
        : type(QtEasingCurve::Linear),
          config(0),
          func(&easeNone)
    { }

    QtEasingCurve::Type type;
    QtEasingCurveFunction *config;
    Function func;
};

struct ElasticEase : public QtEasingCurveFunction
{
    ElasticEase(Type type)
        : QtEasingCurveFunction(type, 0.3f, 1.0f)
    { }

    QtEasingCurveFunction *copy() const
    {
        ElasticEase *rv = new ElasticEase(_t);
        rv->_p = _p;
        rv->_a = _a;
        return rv;
    }

    float value(float t, float b, float c, float d)
    {
        if (t==0) return b;
        float t_adj = (float)t / (float)d;
        if (t_adj==1) return b+c;

        qreal p = _p?_p:(d * 0.3f);

        qreal a;
        qreal s;

        if(_a < 0 || _a < ::fabs(c)) {
            a = c;
            s = p / 4.0f;
        } else {
            a = _a;
            s = p / (2 * M_PI) * ::asin(c / a);
        }

        qreal exp = -10.0;
        if(_t == In) {
            t_adj -= 1.0f;
            exp = 10.0;
            c = 0;
        }

        return (a*::pow(2, exp * t_adj) * ::sin( (t_adj*d-s)*(2*M_PI)/p ) + c + b) * (_t == In ? -1 : 1);
    }
};

struct BounceEase : public QtEasingCurveFunction
{
    BounceEase(Type type)
        : QtEasingCurveFunction(type, 0.3f, 1.0f)
    { }

    QtEasingCurveFunction *copy() const
    {
        BounceEase *rv = new BounceEase(_t);
        rv->_a = _a;
        return rv;
    }

    float value(float t, float b, float c, float d)
    {
        if(In == _t)
            return c - bounce(d - t, 0, c, d) + b;
        else
            return bounce(t, b, c, d);
    }

    float bounce(float t, float b, float c, float d)
    {
        float t_adj = (float)t / (float)d;
        float amp = (_a < 0) ? c:_a;
        if ((t_adj) < (1/2.75)) {
            if(c == 0. && _a != -1.0) {
                t_adj -= (0.5f/2.75f);
                return -amp * (1. - (30.25*t_adj*t_adj)) + b;
            } else {
                return c*(7.5625*t_adj*t_adj) + b;
            }
        } else if (t_adj < (2/2.75)) {
            t_adj -= (1.5f/2.75f);
            return -amp * (1. - (7.5625*t_adj*t_adj + .75)) + (b + c);
        } else if (t_adj < (2.5/2.75)) {
            t_adj -= (2.25f/2.75f);
            return -amp * (1. - (7.5625*t_adj*t_adj + .9375)) + (b + c);
        } else {
            t_adj -= (2.65f/2.75f);
            return -amp * (1. - (7.5625*t_adj*t_adj + .984375)) + (b + c);
        }
    }
};

static Function curveToFunc(QtEasingCurve::Type curve)
{
    switch(curve) {
    case QtEasingCurve::Linear:
        return &easeNone;
    case QtEasingCurve::InQuad:
        return &easeInQuad;
    case QtEasingCurve::OutQuad:
        return &easeOutQuad;
    case QtEasingCurve::InOutQuad:
        return &easeInOutQuad;
    case QtEasingCurve::OutInQuad:
        return &easeOutInQuad;
    case QtEasingCurve::InCubic:
        return &easeInCubic;
    case QtEasingCurve::OutCubic:
        return &easeOutCubic;
    case QtEasingCurve::InOutCubic:
        return &easeInOutCubic;
    case QtEasingCurve::OutInCubic:
        return &easeOutInCubic;
    case QtEasingCurve::InQuart:
        return &easeInQuart;
    case QtEasingCurve::OutQuart:
        return &easeOutQuart;
    case QtEasingCurve::InOutQuart:
        return &easeInOutQuart;
    case QtEasingCurve::OutInQuart:
        return &easeOutInQuart;
    case QtEasingCurve::InQuint:
        return &easeInQuint;
    case QtEasingCurve::OutQuint:
        return &easeOutQuint;
    case QtEasingCurve::InOutQuint:
        return &easeInOutQuint;
    case QtEasingCurve::OutInQuint:
        return &easeOutInQuint;
    case QtEasingCurve::InSine:
        return &easeInSine;
    case QtEasingCurve::OutSine:
        return &easeOutSine;
    case QtEasingCurve::InOutSine:
        return &easeInOutSine;
    case QtEasingCurve::OutInSine:
        return &easeOutInSine;
    case QtEasingCurve::InExpo:
        return &easeInExpo;
    case QtEasingCurve::OutExpo:
        return &easeOutExpo;
    case QtEasingCurve::InOutExpo:
        return &easeInOutExpo;
    case QtEasingCurve::OutInExpo:
        return &easeOutInExpo;
    case QtEasingCurve::InCirc:
        return &easeInCirc;
    case QtEasingCurve::OutCirc:
        return &easeOutCirc;
    case QtEasingCurve::InOutCirc:
        return &easeInOutCirc;
    case QtEasingCurve::OutInCirc:
        return &easeOutInCirc;
    case QtEasingCurve::InElastic:
        return &easeInElastic;
    case QtEasingCurve::OutElastic:
        return &easeOutElastic;
    case QtEasingCurve::InOutElastic:
        return &easeInOutElastic;
    case QtEasingCurve::OutInElastic:
        return &easeOutInElastic;
    case QtEasingCurve::InBack:
        return &easeInBack;
    case QtEasingCurve::OutBack:
        return &easeOutBack;
    case QtEasingCurve::InOutBack:
        return &easeInOutBack;
    case QtEasingCurve::OutInBack:
        return &easeOutInBack;
    case QtEasingCurve::InBounce:
        return &easeInBounce;
    case QtEasingCurve::OutBounce:
        return &easeOutBounce;
    case QtEasingCurve::InOutBounce:
        return &easeInOutBounce;
    case QtEasingCurve::OutInBounce:
        return &easeOutInBounce;
    // Internal for, compatibility with QTimeLine only ??
    case QtEasingCurve::InCurve:
        return &easeInCurve;
    case QtEasingCurve::OutCurve:
        return &easeOutCurve;
    case QtEasingCurve::SineCurve:
        return &easeSineCurve;
    case QtEasingCurve::CosineCurve:
        return &easeCosineCurve;
    default:
        return 0;
    };
}

static QtEasingCurveFunction *curveToFunctionObject(QtEasingCurve::Type type)
{
    QtEasingCurveFunction *curveFunc = 0;
    if (type == QtEasingCurve::InElastic) {
        curveFunc = new ElasticEase(ElasticEase::In);
    } else if (type == QtEasingCurve::OutElastic) {
        curveFunc = new ElasticEase(ElasticEase::Out);
    } else if (type == QtEasingCurve::OutBounce) {
        curveFunc = new BounceEase(BounceEase::Out);
    } else if (type == QtEasingCurve::InBounce) {
        curveFunc = new BounceEase(BounceEase::In);
    } else {
        // default
        curveFunc = new QtEasingCurveFunction(QtEasingCurveFunction::In, 0.3f, 1.0f);     // ### 
    }
    return curveFunc;
}

/*!
    Constructs an easing curve of the given \a type.
 */
QtEasingCurve::QtEasingCurve(Type type)
    : d_ptr(new QtEasingCurvePrivate)
{
    setType(type);
}

/*!
    Constructs an easing curve of the given \a type, with the given \a
    amplitude and \a period. You only have to use this constructor if 
    \a type is one of QtEasingCurve::InElastic, QtEasingCurve::OutElastic, 
    QtEasingCurve::InBounce or QtEasingCurve::OutBounce.
 */
QtEasingCurve::QtEasingCurve(Type type, qreal amplitude, qreal period)
    : d_ptr(new QtEasingCurvePrivate)
{
    setType(type);
    setAmplitude(amplitude);
    setPeriod(period);
}

/*!
    Construct a copy of \a other.
 */
QtEasingCurve::QtEasingCurve(const QtEasingCurve &other)
: d_ptr(new QtEasingCurvePrivate)
{
    // ### non-atomic, requires malloc on shallow copy
    *d_ptr = *other.d_ptr;
    if(other.d_ptr->config) 
        d_ptr->config = other.d_ptr->config->copy();
}

/*!
    Destructor.
 */

QtEasingCurve::~QtEasingCurve()
{
    delete d_ptr;
}

/*!
    Copy \a other.
 */
QtEasingCurve &QtEasingCurve::operator=(const QtEasingCurve &other)
{
    // ### non-atomic, requires malloc on shallow copy
    if (d_ptr->config) {
        delete d_ptr->config;
        d_ptr->config = 0; 
    }

    *d_ptr = *other.d_ptr;
    if(other.d_ptr->config)
        d_ptr->config = other.d_ptr->config->copy();

    return *this;
}

/*!
    Compare this easing curve with \a other.
 */
bool QtEasingCurve::operator==(const QtEasingCurve &other) const
{
    // ### this doesn't catch correctly the config content
    return d_ptr->config == other.d_ptr->config && d_ptr->func == other.d_ptr->func;
}

/*!
    Returns the amplitude. This is not applicable for all curve types.
    It is only applicable if type()  is one of QtEasingCurve::InElastic, QtEasingCurve::OutElastic, 
    QtEasingCurve::InBounce or QtEasingCurve::OutBounce.
 */
qreal QtEasingCurve::amplitude() const
{
    return d_ptr->config ? d_ptr->config->_a : 1.0;
}

/*!
    Sets the amplitude to \a amplitude.
    
    \sa amplitude()
*/
void QtEasingCurve::setAmplitude(qreal amplitude)
{
    if (!d_ptr->config)
        d_ptr->config = curveToFunctionObject(d_ptr->type);        
    d_ptr->config->_a = amplitude;
}

/*!
    Returns the period. This is not applicable for all curve types.
    It is only applicable if type()  is one of QtEasingCurve::InElastic or QtEasingCurve::OutElastic.
 */
qreal QtEasingCurve::period() const
{
    return d_ptr->config ? d_ptr->config->_p : 0.3;
}

/*!
    Sets the period to \a period.
    
    \sa period()
*/
void QtEasingCurve::setPeriod(qreal period)
{
    if (!d_ptr->config)
        d_ptr->config = curveToFunctionObject(d_ptr->type);        
    d_ptr->config->_p = period;
}

/*!
    Returns the type of the easing curve.
*/
QtEasingCurve::Type QtEasingCurve::type() const
{
    return d_ptr->type;
}

/*!
    Sets the type of the easing curve to \a type.
*/
void QtEasingCurve::setType(Type type)
{
    qreal amp = -1.0;
    qreal period = -1.0;
    
    if (d_ptr->config) {
        amp = d_ptr->config->_a;
        period = d_ptr->config->_p;
        delete d_ptr->config;
        d_ptr->config = 0;
    }

    if (isConfigFunction(type) || (amp != -1.0) || (period != -1.0)) {
        d_ptr->config = curveToFunctionObject(type);
        if (amp)
            d_ptr->config->_a = amp;
        if (period)
            d_ptr->config->_p = period;
    } else {
        d_ptr->func = curveToFunc(type);
        if (!d_ptr->func) {
            qWarning("QtEasingCurve: Invalid curve type %d", type);
            return;
        }
    }
    d_ptr->type = type;
}

/*!
    Return the effective progress for the easing curve at \a step.
    While  \a step must be between 0 and 1, the returned effective progress
    can be outside those bounds. For instance, QtEasingCurve::InBack will
    return negative values in the beginning of the function.
 */
qreal QtEasingCurve::valueForStep(qreal step) const
{
    step = qBound<qreal>(0, step, 1);
    if (d_ptr->config)
        return d_ptr->config->value(step, 0, 1, 1);
    else
        return d_ptr->func(step, 0, 1, 1);
}

QT_END_NAMESPACE
