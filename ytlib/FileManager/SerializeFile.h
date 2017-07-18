#pragma once

#include <ytlib/FileManager/FileBase.h>
#include <ytlib/SupportTools/Serialize.h>

namespace wtlib
{
	//ʹ��boost���л����ļ���
	//T��Ҫ֧�����л�
	template <class T>
	class SerializeFile :public FileBase<T> {
	public:
		SerializeFile():FileBase(){}
		virtual ~SerializeFile() {}
	protected:
		
		virtual bool GetFileObj() {
			if (!CreateFileObj()) return false;
			tpath path = tGetAbsolutePath(m_filepath);
			return Deserialize_f(*m_fileobj, path.string<tstring>());
		}
		virtual bool SaveFileObj() {
			return Serialize_f(*m_fileobj, m_filepath);
		}
		
	};
}
