#pragma once

#include <ytlib/FileManager/FileBase.h>
#include <ytlib/SupportTools/Serialize.h>

namespace ytlib
{
	//ʹ��boost���л����ļ���
	//T��Ҫ֧�����л�
	template <class T>
	class SerializeFile :public FileBase<T> {
	public:
		SerializeFile():FileBase<T>(){}
		virtual ~SerializeFile() {}
	protected:
		
		virtual bool GetFileObj() {
			if (!FileBase<T>::CreateFileObj()) return false;
			tpath path = tGetAbsolutePath(FileBase<T>::m_filepath);
			return Deserialize_f(*FileBase<T>::m_fileobj, path.string<tstring>());
		}
		virtual bool SaveFileObj() {
			return Serialize_f(*FileBase<T>::m_fileobj, FileBase<T>::m_filepath);
		}
		
	};
}
