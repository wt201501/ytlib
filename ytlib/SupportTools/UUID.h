/**
 * @file UUID.h
 * @brief UUID
 * @details 基于boost的UUID
 * @author WT
 * @email 905976782@qq.com
 * @date 2019-07-26
 */
#pragma once

#include <ytlib/Common/Util.h>
#include <ytlib/Common/TString.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ytlib
{
	//生成UUID
	static tstring GenerateUUID(void) {
		boost::uuids::random_generator rgen;
#ifdef UNICODE
		return boost::uuids::to_wstring(rgen());
#else
		return boost::uuids::to_string(rgen());
#endif // UNICODE

	}



}

  
