/**
 * @file RouteGuide.h
 * @brief The routing guide read
 * @author Keren Zhu
 * @date 02/11/2019
 */

#ifndef ROUTE_GUIDE_H_
#define ROUTE_GUIDE_H_

#include <cmath> // std::pow
#include "src/global/global.hpp"
#include "src/util/Vector2D.h"

PROJECT_NAMESPACE_START

class CirDB;

/// @class PROJECT_NAMESPACE::RouteGuide
/// @brief routing guide for nets
class RouteGuide
{
    public:
        explicit RouteGuide() = default;
        explicit RouteGuide(UInt_t numX, UInt_t numY, Int_t maxRange, UInt_t numNets)
        : _numX(numX), _numY(numY), _maxRange(maxRange)
        {
            _guides.resize(numNets);
            _hasGuide.resize(numNets, false);
            for (auto &map : _guides)
            {
                map.resize(numX, numY, 0);
            }
            _avgProb.resize(numX, numY);
        }
        /*------------------------------*/ 
        /* Setter                       */
        /*------------------------------*/ 
        /// @brief a guide value
        /// @param first: the routing probability of a net at a grid cell
        /// @param second: net index
        /// @param thrid: x-th grid cell
        /// @param fourth: y-th grid cell
        void setGuide(Int_t value, UInt_t netIdx, UInt_t x, UInt_t y) { _guides.at(netIdx).at(x, y) = value; }
        /// @brief set whether the guide mode is enabled
        void setEnable(bool enable) { _enable = enable; }
        /// @brief set whether a net has guide
        /// @param first: the index of the net
        /// @param second: whether the net has guide
        void setNetHasGuide(UInt_t netIdx, bool hasGuide) { _hasGuide.at(netIdx) = hasGuide; }
        /*------------------------------*/ 
        /* Getter                       */
        /*------------------------------*/ 
        /// @brief get whether the net has guide
        /// @param the idnex of the net
        /// @return whether the net has guide
        bool hasGuide(UInt_t netIdx) const { return _hasGuide.at(netIdx); }
        /// @brief get the raw guidance probability
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return raw int probability
        Int_t probability(UInt_t netIdx, UInt_t x, UInt_t y) const { return _guides.at(netIdx).at(x, y); }
        /// @brief get the float guidance probability
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return probability as float of 0-1.0
        Float_t probabilityFloat(UInt_t netIdx, UInt_t x, UInt_t y) const { return static_cast<Float_t>(_guides.at(netIdx).at(x, y)) / static_cast<Float_t>(_maxRange); }
        /// @brief get the cost 
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost as inverse probability
        Int_t cost(UInt_t netIdx, UInt_t x, UInt_t y) const 
        { 
            if (!_enable) { return 0; }
            Int_t violateCost = this->violateCost(netIdx, x, y);
            Int_t congestCost = this->congestCost(netIdx, x, y);
            //DBG("violate %d , congest %d \n", violateCost, congestCost);
            return violateCost + congestCost;
            //return ( _maxRange - _guides.at(netIdx).at(x, y) ) * GUIDE_COST_MULTI  ;
            //+ std::max((_avgProb.at(x, y) - _guides.at(netIdx).at(x, y) ), 0) * GUIDE_COST_MULTI; 
        }
        /// @brief just the guidance following, can be negative
        Int_t guideConfi(UInt_t netIdx, UInt_t x, UInt_t y) const;
        /// @brief whether the guide is condfident that the net should routed in some grid
        /// @param first: index of the net
        /// @param second: index of the grid
        bool confident(UInt_t netIdx, Point<UInt_t> grid) const { return _guides.at(netIdx).at(grid) > _maxRange / 5; }
        /// @brief whether this object has been initialized
        /// @return whether this object has been initialized
        bool isInit() const { return _guides.size() != 0; }
        /// @brief get the num of x cells
        /// @return number of x cells
        UInt_t numX() const { return _numX; }
        /// @brief get the num of y cells
        /// @return number of y cells
        UInt_t numY() const { return _numY; }
        /// @brief get the max probability value range
        /// @return the range of probability value
        Int_t maxRange() const { return _maxRange; }
        /// @brief init average probability
        void initAverageProb(CirDB &db);

    private:
        /// @brief the cost for violating the guidance
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost of violating the guidance
        Int_t violateCost(UInt_t netIdx, UInt_t x, UInt_t y) const;
        /// @brief the cost for taking the usage of other nets' guidance
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost of taking the usage of other nets' guidance
        Int_t congestCost(UInt_t netIdx, UInt_t x, UInt_t y) const;
        
    private:
        std::vector<Vector2D<Int_t>> _guides; ///< Guide for nets, saved as probability
        UInt_t _numX = MAX_UINT; ///< Number of x grids in guide
        UInt_t _numY = MAX_UINT; ///< number of y grids in guide
        Int_t _maxRange = MAX_INT; ///< Range of probability
        Vector2D<Int_t> _avgProb; ///< Average _guide value
        bool _enable = true; ///< Whether the route guide is enabled
        std::vector<bool> _hasGuide; /// Whether each net has guide
};

inline Int_t RouteGuide::guideConfi(UInt_t netIdx, UInt_t x, UInt_t y) const
{
    if (!_enable)
    {
        return 0;
    }
    /// Sigmoid 99.97226 +  (9.870109 - 99.97226) / (1 + (x / 0.385189) ^ 4.73)
    /// Ratio 0 - 100 99.97226 + (9.870109 - 99.97226) / (1 + (x / 38.5189)^4.73)
    if (!_hasGuide.at(netIdx))
    {
        return 0;
    }
    Int_t ratio = _guides.at(netIdx).at(x, y) * 100 / _maxRange;
    return 99.76226 - 90 / ( 1 + std::pow((ratio / 38.5198), 4.73));
}

inline Int_t RouteGuide::violateCost(UInt_t netIdx, UInt_t x, UInt_t y) const 
{
    /// Ratio 0-100. -100 + 718.2 / 2 ^ (ratio/ 37)
    if (!_hasGuide.at(netIdx))
    {
        return 0;
    }
    Int_t ratio = _guides.at(netIdx).at(x, y) * 100 / _maxRange;
    return -100 + 718.2 / std::pow(2, (ratio / 37));
}

inline Int_t RouteGuide::congestCost(UInt_t netIdx, UInt_t x, UInt_t y) const 
{
    if (_hasGuide.at(netIdx))
    {
        return std::max((_avgProb.at(x, y) - _guides.at(netIdx).at(x, y)), 0) * 3 / _maxRange ;
    }
    else
    {
        return (_avgProb.at(x, y) ) * 100 / _maxRange;
    }
}

PROJECT_NAMESPACE_END

#endif //ROUTE_GUIDE_H_
