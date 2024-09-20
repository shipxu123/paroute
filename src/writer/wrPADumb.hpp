/**
 * @file   wrDumb.hpp
 * @brief  Output file writer - Dumb
 * @author Hao Chen
 * @date   12/11/2019
 *
 **/

#ifndef _WR_PADUMB_HPP_
#define _WR_PADUMB_HPP_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>

#include "src/db/dbCir.hpp"
#include "src/writer/wrGds.hpp"

PROJECT_NAMESPACE_START

class PADumbWriter {
 public:
	PADumbWriter(const CirDB& c)
		: _cir(c) {}
	~PADumbWriter() {}

	void write(const String_t& placementFilename, const String_t& outputFilename);

 private:
	const CirDB& _cir;
	/////////////////////////////////////////
	//    Private functions                //
	/////////////////////////////////////////
	typedef GdsParser::GdsDB::GdsDB   GdsDB;
	typedef GdsParser::GdsDB::GdsCell GdsCell;
	typedef boost::polygon::point_data<Int_t> GdsPoint;

	bool      readPlacementGds(const String_t& filename, GdsParser::GdsDB::GdsDB& gdsDB);
	String_t  topCellName(const GdsDB& gdsDB);
	void      writeBox2Cell(FILE* fout, const Int_t layerIdx, const Box<Int_t>& box, const Float_t scale);
	void      writeTxt2Cell(FILE* fout, const Int_t layerIdx, const Point<Int_t>& pt, const Float_t scale);

};

PROJECT_NAMESPACE_END

#endif /// _WR_PADUMB_HPP_