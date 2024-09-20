/**
 * @file   wrPADumb.cpp
 * @brief  Output file writer - PADumb
 * @author Hao Chen
 * @date   12/11/2019
 *
 **/
#include "wrDumb.hpp"
#include "wrPADumb.hpp"

PROJECT_NAMESPACE_START

void PADumbWriter::write(const String_t& placementFilename, const String_t& outputFilename) {
  GdsDB gdsDB;

  if (!readPlacementGds(placementFilename, gdsDB)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Write GDS Layout Failed!\n", __func__);
    return;
  }
  
  const Int_t gds_unit = std::round(1e-6 / gdsDB.precision());
  const Int_t db_unit = _cir.lef().units().databaseNumber();
  const Float_t scale =  (Float_t)gds_unit / db_unit;

  // find boundaries
  String_t topCellName = this->topCellName(gdsDB);
  auto flatCell = gdsDB.extractCell(topCellName);
  Box<Int_t> bbox;
  for (const auto& obj : flatCell.objects()) {
    GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, ExtractBBoxAction(bbox));
  }

  FILE* fout = fopen(outputFilename.c_str(), "w");

  fprintf(fout, "num net %d\n", (Int_t)_cir.numNets());
  fprintf(fout, "%d %d %d %d\n", bbox.xl(),
                                 bbox.yl(),
                                 bbox.xh(),
                                 bbox.yh());

  UInt_t i;
  const Net* cpNet;
  Cir_ForEachNetC(_cir, cpNet, i) {
    fprintf(fout, "%s %d %d\n", cpNet->name().c_str(), i, (Int_t)cpNet->numPins());

    const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vWires = cpNet->vWires();
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vWires)
    {
      const Box<Int_t>& box = pair.first;
      const Int_t layerIdx = pair.second;
      if (layerIdx > (Int_t)_cir.lef().routingLayerIdx2LayerIdx(5))
      {
        writeBox2Cell(fout, layerIdx + 1, box, scale);
      }
      else
      {
        writeBox2Cell(fout, layerIdx + 1, box, scale);
      }
    }
    if (cpNet->bIOPort()) {
      const Pin& pin = _cir.pin(cpNet->pinIdx(0));
      const Int_t layerIdx = pin.minLayerIdx();
      const Box<Int_t>& box = pin.box(layerIdx, 0);
      writeTxt2Cell(fout, layerIdx + 1, box.center(), scale);
    }
    fprintf(fout, "666\n");
  }
  fclose(fout);
}

void PADumbWriter::writeBox2Cell(FILE* fout, const Int_t layerIdx, const Box<Int_t>& box, const Float_t scale) {
  fprintf(fout, "%d %d %d %d %d\n", layerIdx,
                                 (Int_t)(box.xl() * scale),
                                 (Int_t)(box.yl() * scale),
                                 (Int_t)(box.xh() * scale),
                                 (Int_t)(box.yh() * scale));
}


void PADumbWriter::writeTxt2Cell(FILE* fout, const Int_t layerIdx, const Point<Int_t>& pt, const Float_t scale) {
  fprintf(fout, "%d %d %d\n",  130 + layerIdx,
                                 (Int_t)(pt.x() * scale),
                                 (Int_t)(pt.y() * scale));
}

/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////
bool PADumbWriter::readPlacementGds(const String_t& filename, GdsDB& gdsDB) {
  GdsParser::GdsDB::GdsReader gdsr(gdsDB);
  if (!gdsr(filename)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Cannot open placement file %s!\n", __func__, filename.c_str());
    return false;
  }

  return true;
}

String_t PADumbWriter::topCellName(const GdsDB& gdsDB) {
  UMap_t<String_t, bool> cellNameFoundMap;
  const auto& cells = gdsDB.cells();
  for (Int_t i = cells.size() - 1; i >= 0; --i) {
    cellNameFoundMap[cells.at(i).name()] = false;
  }
  for (const auto& cell : cells) {
    for (const auto& obj : cell.objects()) {
      String_t name;
      GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, GetSRefNameAction(name));
      if (name != "") {
        cellNameFoundMap[name] = true;
      }
    }
  }
  for (const auto& pair : cellNameFoundMap) {
    if (pair.first != "" and !pair.second) {
      return pair.first;
    }
  }
  return "";
}


PROJECT_NAMESPACE_END
