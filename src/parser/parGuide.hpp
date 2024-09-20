/**
 * @file parguide.h
 * @brief parsing in the guide for the routing
 * @author Peng Xu
 * @date 02/11/2019
 */

#ifndef _PAR_GUIDE_HPP_
#define _PAR_GUIDE_HPP__

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

/// @class project_namespace::parserguide
/// @brief parser for the global routing guidance
class GuideReader
{
    public:
        explicit GuideReader(CirDB &c) : _cir(c) {}
        /*------------------------------*/ 
        /* Parsing wrapper              */
        /*------------------------------*/ 
        /// @brief parse the guide from a configure file
        /// @return if successful
        bool parse(const String_t& configFile);

    private:
        /// @brief parse a routing guide file
        /// @return if successful
        bool parseGuideFile(const String_t &guideFile, UInt_t netIdx);
        /// @brief init the RouteGuide in cir
        void initRouteGuide(UInt_t numX, UInt_t numY, Int_t range);
        /// @brief set the net name map
        void initNetNameMap();

    private:
        CirDB& _cir;
        std::unordered_map<String_t, UInt_t> _netNameMap;
};

PROJECT_NAMESPACE_END

#endif /// _PAR_GUIDE_H_
