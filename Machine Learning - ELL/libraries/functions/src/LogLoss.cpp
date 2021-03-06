/**
 * Microsoft - Modern Information Technology
 * https://github.com/microsoft/ELL/blob/master/libraries/functions/src/LogLoss.cpp
 *
 *  Created on: June 15, 2019
 *  Student (MIG Virtual Developer): Tung Dang
 */

#include "LogLoss.h"
#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>
#include <cmath>

namespace ell
{
    namespace functions
    {
        double LogLoss::operator()(double prediction, double label) const 
        {
            double margin = prediction * label;
            if (margin <= -18.0)
            {
                return -prediction * label;
            }
            else
            {
                 return std::log1p(std::exp(-margin));
            }
        }

        double LogLoss::GetDerivative(double prediction, double label) const 
        {
            double margin = prediction * label;
            if (margin <= 0.0)
            {
                double exp_margin = std::exp(margin);
                return -label / (1.0 + exp_margin);
            }
            else 
            {
                double exp_neg_margin = std::exp(-margin);
                return -label * exp_neg_margin / (1.0 + exp_neg_margin);
            }
        }

        double LogLoss::Conjugate(double dual, double label) const 
        {
            double x = dual * label;
            DEBUG_THROW(x < -1 || x > 0, utilities::InputException(utilities::InputExceptionErrors::invalidArgument, 
                            "dual times label must be in [-1,0]"));
            if (x >= -_conjugateBoundary || x <= -1 + _conjugateBoundary)
            {
                return 0.0;
            }
            else 
            {
                return (-x) * std::log(-x) + (1.0 + x) * std::log1p(x);
            }
        }

        double LogLoss::ConjugateProx(double sigma, double dual, double label) const 
        {
            double lowerBound = _conjugateBoundary - 1.0;
            double upperBound = - _conjugateBoundary;

            double x = dual * label;
            double b = std::fmin(upperBound, std::fmax(lowerBound, x));
            double f = 0;
            double df = 0;
            for (size_t k = 0; k < _conjugateProxMaxIterations; ++k)
            {
                f = b - x + sigma * std::log((1.0 + b) / (-b));
                if (std::fabs(f) <= _conjugateProxDesiredPrecision) break;
                df = 1.0 - sigma / (b * (1.0 + b));
                b -= f/df;
                b = std::fmin(upperBound, std::fmax(lowerBound, b));
            }
            return b * label;
        }
    }
}
