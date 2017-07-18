#pragma once
#include <ytlib/ProcessManager/ProcessBase.h>
#include <ytlib/SupportTools/ChannelBase.h>
#include <ytlib/Common/TString.h>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <mutex>

namespace wtlib {

	//����������ģ�壬�첽����
	template<class _HostInfo,//������Ϣ��ʽ
	class _RecvData,
	class _TransData>
	class NetAdapterBase : public ProcessBase {
	public:

		typedef std::pair<_TransData, std::vector<tstring>> _TransData_dst;//���ݼ���Ŀ�ĵ��б�֧��Ⱥ�����ظ�����

		/*������
		myid_������id
		hostInfo_������������Ϣ
		recvcb_���������ݻص�
		*/
		NetAdapterBase(const tstring& myid_, const _HostInfo & hostInfo_, 
			std::function<void(_RecvData &)> recvcb_):
			ProcessBase(),
			m_myid(myid_),
			m_bStopFlag(false),
			m_TChannel(std::bind(&NetAdapterBase::_sendfun,this, std::placeholders::_1),1,1000),
			m_RChannel(recvcb_,1,1000)
		{
			m_mapHostInfo[m_myid] = hostInfo_;
		};
		virtual ~NetAdapterBase() {
			stop();
		}

		//�ṩ�����ͷ��ķ��ͽӿ�
		inline bool Send(const _TransData & Tdata_,const std::vector<tstring>& dst_) {
			return is_running && m_TChannel.Add(std::move(std::make_pair<_TransData, std::vector<tstring>>(Tdata_, dst_)));
		}
		

		//hostinfo����
		inline _HostInfo GetMyHostInfo() {
			std::lock_guard<std::mutex> lck(m_hostInfoMutex);
			return m_mapHostInfo[m_myid];
		}
		inline _HostInfo GetHostInfo(const tstring & hostid_) {
			std::lock_guard<std::mutex> lck(m_hostInfoMutex);
			std::map<tstring, _HostInfo>::iterator itr = m_mapHostInfo.find(hostid_);
			if (itr != m_mapHostInfo.end()) return itr->second;
			else return _HostInfo();//���쳣��
		}
		//��������info�����򸲸ǣ��������
		bool SetHost(const tstring & hostid_, const _HostInfo & hostInfo_) {
			std::lock_guard<std::mutex> lck(m_hostInfoMutex);
			m_mapHostInfo[hostid_] = hostInfo_;
		}
		//�Ƴ�����info�������Ƴ�����info
		bool RemoveHost(const tstring & hostid_) {
			if (hostid_ == m_myid) return false;
			std::lock_guard<std::mutex> lck(m_hostInfoMutex);
			std::map<tstring, _HostInfo>::iterator itr = m_mapHostInfo.find(hostid_);
			if (itr == m_mapHostInfo.end()) return false;
			m_mapHostInfo.erase(itr);
			return true;
		}

	protected:
		//�ṩ�������̵߳Ľ������ݽӿڡ����������
		inline bool Recv(const _RecvData & Rdata_) {
			return is_running && m_RChannel.Add(Rdata_);
		}
		//���ͺ���
		void _sendfun(const _TransData_dst& Tdata_) {
			std::vector<tstring> &dstVec = Tdata_.second;
			size_t len = dstVec.size();
			for (size_t ii = 0; ii < len; ii++) {
				_finalsend(std::move(Tdata_.first), GetHostInfo(dstVec[ii]));
			}
		}
		virtual void _finalsend(const _TransData& data_, const _HostInfo& hostinfo_) = 0;//���շ��ͺ���

		const tstring m_myid;//����id������֮���޷��޸�

		std::mutex m_hostInfoMutex;
		std::map<tstring, _HostInfo> m_mapHostInfo;//�����б�id-info

		std::function<void(_RecvData &)> m_receiveCallBack;//�ص�
		std::atomic_bool m_bStopFlag;

		ChannelBase<_TransData_dst> m_TChannel;//����ͨ��
		ChannelBase<_RecvData> m_RChannel;//����ͨ��
		

	};
}