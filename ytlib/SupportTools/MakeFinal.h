#pragma once

namespace ytlib
{
	/*
	����һ���޷����̳е��ࣺ
	class FinalClass2 : virtual public MakeFinal<FinalClass2>{
	public :
		FinalClass2() {}
		~FinalClass2() {}

	};
	*/
	template <typename T>
	class MakeFinal {
		friend T;
	private:
		MakeFinal() {}
		~MakeFinal() {}
	};


}


