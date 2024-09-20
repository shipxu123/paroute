/**
 * @file   dbLef.cpp
 * @brief  Technology configuration from Lef file
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#include "dbLef.hpp"

PROJECT_NAMESPACE_START

void LefDB::setVersionStr(const String_t& v) {
  _versionStr = v;
}

void LefDB::setVersion(const Float_t v) {
  _version = v;
}

void LefDB::setDividerChar(const String_t& v) {
  _dividerChar = v;
}

void LefDB::setManufacturingGrid(const Float_t v) {
  _manufacturingGrid = v;
}

void LefDB::setClearanceMeasure(const String_t& v) {
  _clearanceMeasure = v;
}

void LefDB::setBusbitChars(const String_t& v) {
  _busbitChars = v;
}

void LefDB::addSite(const LefSite& s) {
  _mStr2SiteIdx[s.name()] = _vSites.size();
  _vSites.emplace_back(s);
}

void LefDB::addImplantLayer(const LefImplantLayer& l) {
  _mStr2AllLayerIdx[l.name()] = _vAllLayers.size();
  _mStr2ImplantLayerIdx[l.name()] = _vImplantLayers.size();
  _vImplantLayerIdx2AllLayerIdx.emplace_back(_vAllLayers.size());
  _vAllLayers.emplace_back(LefLayerType::IMPLANT, _vImplantLayers.size());
  _vImplantLayers.emplace_back(l);
}

void LefDB::addMastersliceLayer(const LefMastersliceLayer& l) {
  _mStr2AllLayerIdx[l.name()] = _vAllLayers.size();
  _mStr2MastersliceLayerIdx[l.name()] = _vMastersliceLayers.size();
  _vMastersliceLayerIdx2AllLayerIdx.emplace_back(_vAllLayers.size());
  _vAllLayers.emplace_back(LefLayerType::MASTERSLICE, _vMastersliceLayers.size());
  _vMastersliceLayers.emplace_back(l);
} 

void LefDB::addCutLayer(const LefCutLayer& l) {
  _mStr2AllLayerIdx[l.name()] = _vAllLayers.size();
  _mStr2CutLayerIdx[l.name()] = _vCutLayers.size();
  _vCutLayerIdx2AllLayerIdx.emplace_back(_vAllLayers.size());
  _vAllLayers.emplace_back(LefLayerType::CUT, _vCutLayers.size());
  _vCutLayers.emplace_back(l);
}

void LefDB::addRoutingLayer(const LefRoutingLayer& l) {
  _mStr2AllLayerIdx[l.name()] = _vAllLayers.size();
  _mStr2RoutingLayerIdx[l.name()] = _vRoutingLayers.size();
  _vRoutingLayerIdx2AllLayerIdx.emplace_back(_vAllLayers.size());
  _vAllLayers.emplace_back(LefLayerType::ROUTING, _vRoutingLayers.size());
  _vRoutingLayers.emplace_back(l);
}

void LefDB::addOverlapLayer(const LefOverlapLayer& l) {
  _mStr2AllLayerIdx[l.name()] = _vAllLayers.size();
  _mStr2OverlapLayerIdx[l.name()] = _vOverlapLayers.size();
  _vOverlapLayerIdx2AllLayerIdx.emplace_back(_vAllLayers.size());
  _vAllLayers.emplace_back(LefLayerType::OVERLAP, _vOverlapLayers.size());
  _vOverlapLayers.emplace_back(l);
}

void LefDB::addVia(const LefVia& v) {
  _mStr2ViaIdx[v.name()] = _vVias.size();
  if (v.cutLayerIdx() >= _vvViaIndices.size()) {
    _vvViaIndices.resize(v.cutLayerIdx() + 1);
  }
  _vvViaIndices[v.cutLayerIdx()].emplace_back(_vVias.size());
  _vVias.emplace_back(v);
}

void LefDB::logInfo() const {
  for (UInt_t i = 0; i < _vSites.size(); ++i) {
    _vSites[i].logInfo();
  }
  for (UInt_t i = 0; i < _vCutLayers.size(); ++i) {
    _vCutLayers[i].logInfo();
  }
  for (UInt_t i = 0; i < _vRoutingLayers.size(); ++i) {
    _vRoutingLayers[i].logInfo();
  }
  for (UInt_t i = 0; i < _vVias.size(); ++i) {
    _vVias[i].logInfo();
  }
}

Int_t LefDB::prlSpacing(const Int_t layerIdx, const Int_t wireWidth, const Int_t wirePrl) {
  assert(bRoutingLayer(layerIdx));
  const auto& layerPair = _vAllLayers[layerIdx];
  const auto& layer = _vRoutingLayers[layerPair.second];

  Int_t spacing = 0;
  if (layer.spacingTable().table.size()) {
    for (Int_t i = 0; i < (Int_t)layer.spacingTable().table.size(); ++i) {
      const auto& table = layer.spacingTable().table[i];
      const auto& width = table.first;
      const auto& vSpacings = table.second;

      const Int_t nextWidth = (i < (Int_t)layer.spacingTable().table.size() - 1) ?
                              layer.spacingTable().table[i + 1].first : MAX_INT;

      if (width <= wireWidth and wireWidth < nextWidth) {
        assert(vSpacings.size() == layer.spacingTable().vParallelRunLength.size());
        for (Int_t j = 0; j < (Int_t)vSpacings.size(); ++j) {
          const Int_t prl = layer.spacingTable().vParallelRunLength[j];
          const Int_t nextPrl = (j < (Int_t)vSpacings.size() - 1) ?
                                layer.spacingTable().vParallelRunLength[j + 1] : MAX_INT;

          if (prl <= wirePrl and wirePrl < nextPrl) {
            // FIXME
            //spacing = vSpacings[j];
            spacing = vSpacings.back();
            break;
          }
        }
        break;
      }
    }
  }
  else {
    spacing = layer.spacing(0);
  }
  return spacing;

}

PROJECT_NAMESPACE_END
