#pragma once

#include <ytlib/Common/TString.h>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace ytlib {

	struct RecvData {
		tstring hostID;//���ͷ�����id
		std::vector<std::shared_ptr<std::string> > dataVec;//����
		std::vector<tstring> files;	//���յ��������ļ����ļ���
	};

	struct TransData {
		std::vector<std::shared_ptr<std::string> > dataVec;//����
		std::vector<tstring> files;	//���յ��������ļ����ļ���
		bool delFileFlag;// �������,�Ƿ�ɾ�������ļ�
	};

	//�Ƚ�ͨ�õ������������ӿ�
	class INetAdapter {
	public:
		INetAdapter(){}
		virtual ~INetAdapter(){}

		virtual bool Send(const TransData &) = 0;//TransData�а���������Ϣ								 
		virtual bool Initialize(const std::map<tstring, tstring>&) = 0;//��ʼ��
		virtual bool RegisterReceiveCallBack(std::function<void(RecvData &)>) = 0;
		virtual bool start() = 0;
		virtual bool stop() = 0;

		virtual std::map<tstring, tstring> GetMyHostInfo() = 0;
		virtual std::map<tstring, tstring> GetHostInfo(const tstring &) = 0;
		virtual bool SetHost(const tstring &, const std::map<tstring, tstring> &) = 0;
		virtual bool RemoveHost(const tstring &) = 0;
		
	};



}