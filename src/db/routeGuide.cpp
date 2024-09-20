#include "routeGuide.hpp"
#include "dbCir.hpp"

PROJECT_NAMESPACE_START

void RouteGuide::initAverageProb(CirDB &cir)
{
    if (!_enable)
    {
        return;
    }
    Int_t totalWeight = 0;
    for (UInt_t netIdx = 0; netIdx <  _hasGuide.size(); ++netIdx)
    {
        if (_hasGuide.at(netIdx) && cir.net(netIdx).minWidth() > 0)
        {
            totalWeight += cir.net(netIdx).minWidth();
        }
        else
        {
            _guides.at(netIdx).resize(_numX, _numY, 0);
        }
    }
    if (totalWeight == 0)
    {
         return;
    }
    for (UInt_t x = 0; x < _numX; ++x)
    {
        for (UInt_t y = 0; y < _numY; ++y)
        {
            Int_t sum = 0;
            for (UInt_t netIdx = 0; netIdx < _guides.size(); ++netIdx)
            {
                sum += _guides.at(netIdx).at(x, y) * cir.net(netIdx).minWidth();
            }
            _avgProb.at(x, y) = sum / totalWeight;
        }
    }


}

PROJECT_NAMESPACE_END
