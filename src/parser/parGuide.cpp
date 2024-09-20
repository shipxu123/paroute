#include "parGuide.hpp"

PROJECT_NAMESPACE_START

inline std::string parseFile2Path(String_t str_path) 
{
    size_t pos = str_path.rfind('/');
    if (pos == std::string::npos) return str_path;

    return str_path.substr(0, pos);
}

bool GuideReader::parse(const String_t &configFile)
{
    // Init net name map
    this->initNetNameMap();
    std::ifstream inf(configFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("Routing guide parser:%s: cannot open file: %s \n", __FUNCTION__ , configFile.c_str());
        Assert(false);
        return false;
    }
    String_t lineStr;
    while(std::getline(inf, lineStr))
    {
        std::istringstream iss(lineStr);
        std::vector<String_t> split;
        String_t token;
        while(std::getline(iss, token, ' '))
        {
            split.emplace_back(token);
        }
        if (split.size() != 2)
        {
            ERR("Routing guide parser: wrong syntax \n");
            Assert(false);
            return false;
        }
        if (_netNameMap.find(split.at(0)) == _netNameMap.end())
        {
            ERR("Parse routing guide: net %s is not found! \n", split.at(0).c_str());
            continue;
        }
        UInt_t netIdx = _netNameMap.at(split.at(0));
        if (!parseGuideFile(parseFile2Path(configFile) +"/" + split.at(1), netIdx))
        {
            ERR("Routing guide parser failed \n");
            return false;
        }
    }
    _cir.routeGuide().initAverageProb(_cir);
    return true;
}

bool GuideReader::parseGuideFile(const String_t & guideFile, UInt_t netIdx)
{
    std::ifstream inf(guideFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("Routing guide parser:%s: cannot open file: %s \n", __FUNCTION__ , guideFile.c_str());
        Assert(false);
        return false;
    }
    String_t lineStr;
    // <# of x> <# of y> <range>
    std::getline(inf, lineStr);
    std::istringstream iss(lineStr);
    std::vector<String_t> split;
    String_t token;
    while(std::getline(iss, token, ' '))
    {
        split.emplace_back(token);
    }
    if (split.size() != 3)
    {
        ERR("routing guide parser: wrong syntax \n");
        assert(false);
        return false;
    }
    UInt_t numX = std::stoi(split.at(0));
    UInt_t numY = std::stoi(split.at(1));
    Int_t range = std::stoi(split.at(2));

    // If the routeguide is not intialized, initialize it 
    if (!_cir.routeGuide().isInit())
    {
        initRouteGuide(numX, numY, range);
    }

    // Set the net has guide
    _cir.routeGuide().setNetHasGuide(netIdx, true);

    // Read the probability
    for (UInt_t x = 0; x < numX; ++x)
    {
        for (UInt_t y = 0; y < numY; ++y)
        {
            std::getline(inf, lineStr);
            _cir.routeGuide().setGuide(std::stoi(lineStr), netIdx, x, y);
        }
    }
    // Read the probability
    for (UInt_t y = 0; y < numY; ++y)
    {
        for (UInt_t x = 0; x < numX; ++x)
        {
            std::cout<< _cir.routeGuide().probability(netIdx, x, y)<<" ";
        }
        std::cout<< " end\n";
    }

    return true;
}

void GuideReader::initNetNameMap()
{
    for (UInt_t netIdx = 0; netIdx < _cir.numNets(); ++netIdx)
    {
        _netNameMap[_cir.net(netIdx).name()] = netIdx;
    }
}

void GuideReader::initRouteGuide(UInt_t numX, UInt_t numY, Int_t range)
{
    _cir.routeGuide() = RouteGuide(numX, numY, range, _cir.numNets());
}

PROJECT_NAMESPACE_END
