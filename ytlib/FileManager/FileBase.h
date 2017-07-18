#pragma once

#include <ytlib/Common/FileSystem.h>
#include <ytlib/Common/TString.h>
#include <ytlib/Common/Error.h>
#include <ytlib/SupportTools/Serialize.h>
#include <memory>

namespace wtlib
{
	//Ĭ��Ϊtxt��׺
	template <class T>
	class FileBase
	{
	protected:
		tstring m_filepath;
		bool m_bInitialized;

		//�ļ����ݽ�����Ľṹ��
		std::shared_ptr<T> m_fileobj;

		//����ļ�������ȷ��
		virtual bool CheckFileName(const tstring& filename) const {
			//һ����Ǽ���׺��
			/*
			tstring Suffix = WT_TEXT("txt");
			if (ToLower(filename.substr(filename.length() - Suffix.length(), Suffix.length())) != Suffix) {
				return false;
			}
			*/
			return true;
		}
		//�½�һ���ļ����ݽṹ�塣��һЩ��ʼ���Ĺ���
		virtual bool CreateFileObj() {
			m_fileobj = std::shared_ptr<T>(new T());
			return true;
		}
		//�Ӵ򿪵��ļ��н�����ȡ�ļ����ݽṹ��
		virtual bool GetFileObj() = 0;
		//����ǰ���ļ����ݽṹ�屣��Ϊ�ļ�
		virtual bool SaveFileObj() = 0;
	public:
		FileBase(): m_bInitialized(false), m_fileobj(std::shared_ptr<T>(new T())){}
		virtual ~FileBase() {}

		inline std::shared_ptr<T> GetFileObjPtr() const{
			return m_fileobj;
		}

		//������open�ɹ�������ļ���ýṹ�壬���ճɹ��ŷ���success
		void OpenFile(const tstring& path) {
			//����ļ�������·��
			if (!CheckFileName(path)) {
				throw Exception(Error::ER_ERROR_INVALID_FILENAME);
			}
			tpath filepath = tGetAbsolutePath(path);
			m_filepath = filepath.string<tstring>();
			if (!boost::filesystem::exists(filepath)) {
				throw Exception(Error::ER_ERROR_FILE_NOT_EXIST);
			}
			if (!CreateFileObj()) {
				throw Exception(Error::ER_ERROR_NEW_FILE_FAILED);
			}
			if (!GetFileObj()) {
				throw Exception(Error::ER_ERROR_PARSE_FILE_FAILED);
			}
			m_bInitialized = true;
		}

		//�������½��ṹ��ɹ���ŷ���success
		void NewFile() {
			m_filepath.clear();
			if (!CreateFileObj()) {
				throw Exception(Error::ER_ERROR_NEW_FILE_FAILED);
			}
			m_bInitialized = true;
		}
		//�½��ṹ�岢���浽path��
		void NewFile(const tstring& path) {
			//����ļ�������·��
			if (!CheckFileName(path)) {
				throw Exception(Error::ER_ERROR_INVALID_FILENAME);
			}
			tpath filepath = tGetAbsolutePath(path);
			m_filepath = filepath.string<tstring>();
			if (!CreateFileObj()) {
				throw Exception(Error::ER_ERROR_NEW_FILE_FAILED);
			}
			m_bInitialized = true;
			return(SaveFile());
		}
		//�����ڱ���ɹ������ṹ��д���ļ���ŷ���success
		void SaveFile() {
			if ((m_filepath.empty()) || (!m_bInitialized)) {
				throw Exception(Error::ER_ERROR_INVALID_SAVE);
			}
			tpath filepath = tGetAbsolutePath(m_filepath).parent_path();
			if ((!boost::filesystem::exists(filepath)) && (!boost::filesystem::create_directories(filepath))) {
				throw Exception(Error::ER_ERROR_INVALID_SAVE);
			}
			if (!SaveFileObj()) {
				throw Exception(Error::ER_ERROR_SAVE_FILE_FAILED);
			}
		}

		//���������Ϊ�ɹ������ṹ��д���ļ���ŷ���success
		void SaveFile(const tstring& path) {
			if (!m_bInitialized) {
				throw Exception(Error::ER_ERROR_INVALID_SAVE);
			}
			//����ļ�������·��
			if (!CheckFileName(path)) {
				throw Exception(Error::ER_ERROR_INVALID_FILENAME);
			}
			tpath filepath = tGetAbsolutePath(path);
			m_filepath = filepath.string<tstring>();
			filepath = filepath.parent_path();
			if((!boost::filesystem::exists(filepath))&& (!boost::filesystem::create_directories(filepath))){
				throw Exception(Error::ER_ERROR_INVALID_SAVE);
			}
			if (!SaveFileObj()) {
				throw Exception(Error::ER_ERROR_SAVE_FILE_FAILED);
			}
		}

		inline bool isInitialized() const {
			return m_bInitialized;
		}

		inline tstring GetFilePath() const {
			return m_filepath;
		}
		//ȥ����׺
		tstring GetFileName() const {
			if (m_filepath.empty())	return WT_TEXT("");
			tpath filepath = tGetAbsolutePath(m_filepath);
			tstring fname = filepath.filename().string<tstring>();
			size_t pos = fname.find(WT_TEXT('.'));
			if (pos < fname.length()) {
				fname = fname.substr(0, pos);
			}
			return fname;
		}
		tstring GetFileParentPath() const {
			if (m_filepath.empty()) return WT_TEXT("");
			tpath filepath = tGetAbsolutePath(m_filepath);
			return filepath.parent_path().string<tstring>();
		}
	};
}

